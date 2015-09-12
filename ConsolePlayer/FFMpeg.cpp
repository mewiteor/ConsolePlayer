/*
 * copyright (c) 2015 Mewiteor
 *
 * This file is part of ConsolePlayer.
 *
 * ConsolePlayer is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * ConsolePlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with ConsolePlayer; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include "FFMpeg.h"
#include <memory>
#include "Exception.h"

using namespace std;

CFFMpeg::CFFMpeg(const char* path)
    : m_pFormatCtx(nullptr)
    , m_strPath(path)
    , m_pVideoCodecCtx(nullptr)
    , m_pAudioCodecCtx(nullptr)
    , m_nInWidth(0)
    , m_nInHeight(0)
    , m_nOutWidth(0)
    , m_nOutHeight(0)
    , m_nVideoIndex(-1)
    , m_nAudioIndex(-1)
    , m_nChannels(-1)
    , m_nSampleRate(44100)
    , m_nBitsPerSample(16)
    , m_avSampleFormat(AVSampleFormat::AV_SAMPLE_FMT_S16)
    , m_nVideoFrameCount(0)
{
    av_register_all();
    m_pFormatCtx = avformat_alloc_context();
    if (!m_pFormatCtx)ThrowWin32Error(avformat_alloc_context, ERROR_OUTOFMEMORY);
    int r = avformat_open_input(&m_pFormatCtx, path, nullptr, nullptr);
    if (r) ThrowCError(avformat_open_input, -r);
    r = avformat_find_stream_info(m_pFormatCtx, nullptr);
    if (r < 0) ThrowCError(avformat_find_stream_info, -r);
}

CFFMpeg::~CFFMpeg()
{
    if (m_pVideoCodecCtx)
        avcodec_close(m_pVideoCodecCtx);
    if (m_pAudioCodecCtx)
        avcodec_close(m_pAudioCodecCtx);
    if (m_pFormatCtx)
        avformat_close_input(&m_pFormatCtx);
}

void CFFMpeg::InitVideo()
{
    unsigned int videoindex = 0;
    while (videoindex < m_pFormatCtx->nb_streams)
    {
        if (AVMediaType::AVMEDIA_TYPE_VIDEO == m_pFormatCtx->streams[videoindex]->codec->codec_type)
            break;
        ++videoindex;
    }
    if (videoindex == m_pFormatCtx->nb_streams)
        ThrowMsgError("Didn't find a video stream.");
    m_nVideoIndex = videoindex;
    m_pVideoCodecCtx = m_pFormatCtx->streams[videoindex]->codec;
    auto pCodec = avcodec_find_decoder(m_pVideoCodecCtx->codec_id);
    if (nullptr == pCodec)
        ThrowFunMsgError(avcodec_find_decoder,"Could not open codec.");
    int r = avcodec_open2(m_pVideoCodecCtx, pCodec, nullptr);
    if (r < 0) ThrowCError(avcodec_open2, -r);
    m_nInWidth = m_pVideoCodecCtx->width;
    m_nInHeight = m_pVideoCodecCtx->height;
}

void CFFMpeg::InitAudio()
{
    unsigned int audioindex = 0;
    while (audioindex < m_pFormatCtx->nb_streams)
    {
        if (AVMediaType::AVMEDIA_TYPE_AUDIO == m_pFormatCtx->streams[audioindex]->codec->codec_type)
            break;
        ++audioindex;
    }
    if (audioindex == m_pFormatCtx->nb_streams)
        ThrowMsgError("Didn't find a audio stream.");
    m_nAudioIndex = audioindex;
    m_pAudioCodecCtx = m_pFormatCtx->streams[audioindex]->codec;
    auto pCodec = avcodec_find_decoder(m_pAudioCodecCtx->codec_id);
    if (nullptr == pCodec)
        ThrowFunMsgError(avcodec_find_decoder,"Could not open codec.");
    int r = avcodec_open2(m_pAudioCodecCtx, pCodec, nullptr);
    if (r < 0) ThrowCError(avcodec_open2, -r);
    m_nChannels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
}

void CFFMpeg::SetOutSize(int width, int height)
{
    m_nOutWidth = width;
    m_nOutHeight = height;
}

void CFFMpeg::GetInSize(int &width, int &height) const
{
    width = m_nInWidth;
    height = m_nInHeight;
}

bool CFFMpeg::ReadFrames(bool(*fnFrameProc)(StreamType, uint8_t*, int, int64_t, CAudio&, CVideo&), CAudio& audio, CVideo& video)
{
    int logn = 0;
    if (!fnFrameProc)return true;
    if (!m_nOutWidth)m_nOutWidth = m_nInWidth;
    if (!m_nOutHeight)m_nOutHeight = m_nInHeight;
    auto del_avframe = [](AVFrame* p) {if (p)av_frame_free(&p); };
    unique_ptr<AVFrame, decltype(del_avframe)>
        pFrame(av_frame_alloc(), del_avframe),
        pVideoFrame(av_frame_alloc(), del_avframe);
    auto del_av = [](uint8_t*p) {if (p)av_free(p); };
    unique_ptr<uint8_t, decltype(del_av)>
        out_buffer(m_nVideoIndex < 0 ? nullptr :
            reinterpret_cast<uint8_t*>(av_mallocz(
                avpicture_get_size(AVPixelFormat::PIX_FMT_RGB24, m_nOutWidth,
                    m_nOutHeight))), del_av);
    if (out_buffer)
        avpicture_fill(reinterpret_cast<AVPicture*>(pVideoFrame.get()),
            out_buffer.get(), AVPixelFormat::PIX_FMT_RGB24, m_nOutWidth, m_nOutHeight);

    auto del_packet = [](AVPacket *p) {if (p)av_free(p); };
    unique_ptr<AVPacket, decltype(del_packet)>
        pPacket(reinterpret_cast<AVPacket*>(av_malloc(sizeof(AVPacket))), del_packet);
    if (pPacket) av_init_packet(pPacket.get());

    auto del_sws = [](SwsContext* p) {if (p) sws_freeContext(p); };
    unique_ptr<SwsContext, decltype(del_sws)> img_convert_ctx(
        m_nVideoIndex < 0 ? nullptr :
        sws_getContext(m_nInWidth, m_nInHeight, m_pVideoCodecCtx->pix_fmt,
            m_nOutWidth, m_nOutHeight, AVPixelFormat::PIX_FMT_RGB24,
            SWS_FAST_BILINEAR, nullptr, nullptr, nullptr), del_sws);
    unique_ptr<uint8_t, decltype(del_av)>
        out_buf(m_nAudioIndex < 0 ? nullptr :
            reinterpret_cast<uint8_t*>(av_malloc(MAX_AUDIO_FRAME_SIZE * 4)), del_av);
    auto out_channel_layout = AV_CH_LAYOUT_STEREO;
    int out_nb_samples = m_pAudioCodecCtx ? m_pAudioCodecCtx->frame_size : 0;
    auto out_sample_fmt = m_avSampleFormat;
    auto out_sample_rate = m_nSampleRate;
    auto in_channel_layout = m_pAudioCodecCtx ? av_get_default_channel_layout(m_pAudioCodecCtx->channels) : 0;
    int out_channels = m_nChannels;

    auto del_swr = [](SwrContext* p) {if (p)swr_free(&p); };
    unique_ptr<SwrContext, decltype(del_swr)> au_convert_ctx(
        m_nAudioIndex < 0 ? nullptr :
        swr_alloc_set_opts(swr_alloc(), out_channel_layout, out_sample_fmt, out_sample_rate,
            in_channel_layout, m_pAudioCodecCtx->sample_fmt, m_pAudioCodecCtx->sample_rate, 0, NULL),
        del_swr);
    if (au_convert_ctx) swr_init(au_convert_ctx.get());
    size_t skip_times = 0;
    while (m_pFormatCtx&&pPacket&&pFrame&&av_read_frame(m_pFormatCtx, pPacket.get()) >= 0)
    {
        int got_frame = 0;
        if (m_pVideoCodecCtx&&pPacket->stream_index == m_nVideoIndex)
        {
            int r = avcodec_decode_video2(m_pVideoCodecCtx, pFrame.get(), &got_frame, pPacket.get());
            if (r < 0)ThrowCError(avcodec_decode_video2, -r);
            if (img_convert_ctx)
            {
                if (!got_frame)++skip_times;
                else
                {
                    auto height = sws_scale(img_convert_ctx.get(), pFrame->data, pFrame->linesize,
                        0, m_pVideoCodecCtx->height, pVideoFrame->data, pVideoFrame->linesize);
                    if (fnFrameProc(StreamType::VIDEO,
                        pVideoFrame->data[0], pVideoFrame->linesize[0] * height,
                        m_pVideoCodecCtx->framerate.num
                        ? m_nVideoFrameCount++ * 1000 * m_pVideoCodecCtx->framerate.den / m_pVideoCodecCtx->framerate.num
                        : pFrame->best_effort_timestamp,
                        audio, video))return true;
                }
            }
        }
        else if (m_pAudioCodecCtx&&pPacket->stream_index == m_nAudioIndex)
        {
            int r = avcodec_decode_audio4(m_pAudioCodecCtx, pFrame.get(), &got_frame, pPacket.get());
            if (r < 0)ThrowCError(avcodec_decode_audio4, -r);
            if (au_convert_ctx&&got_frame)
            {
                auto x = out_buf.get();
                int ret = swr_convert(au_convert_ctx.get(), &x, MAX_AUDIO_FRAME_SIZE, (const uint8_t **)pFrame->data, pFrame->nb_samples);
                int out_buffer_size = ret*out_channels*av_get_bytes_per_sample(out_sample_fmt);
                if (fnFrameProc(StreamType::AUDIO, out_buf.get(), out_buffer_size, pFrame->best_effort_timestamp, audio, video))return true;
            }
        }
    }
    while (m_pVideoCodecCtx&&pPacket&&pFrame&&img_convert_ctx&&skip_times--)
    {
        int got_frame = 0;
        int r = avcodec_decode_video2(m_pVideoCodecCtx, pFrame.get(), &got_frame, pPacket.get());
        if (r < 0 || !got_frame) break;
        auto height = sws_scale(img_convert_ctx.get(), pFrame->data, pFrame->linesize,
            0, m_pVideoCodecCtx->height, pVideoFrame->data, pVideoFrame->linesize);
        if (fnFrameProc(StreamType::VIDEO,
            pVideoFrame->data[0], pVideoFrame->linesize[0] * height,
            m_pVideoCodecCtx->framerate.num
            ? m_nVideoFrameCount++ * 1000 * m_pVideoCodecCtx->framerate.den / m_pVideoCodecCtx->framerate.num
            : pFrame->best_effort_timestamp,
            audio, video))return true;
    }
    return false;
}

