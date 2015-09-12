控制台播放器
=============

控制台播放器是一个运行在控制台中的播放器，可播放视频，支持大部分常见视频格式。

## 颜色模式
控制台播放器支持三种颜色模式：
* 黑白模式。此模式显示为16级灰度
* 标准模式。此模式用控制台默认的调色板显示彩色图像
* 自动模式。此模式会计算出每一帧的最佳调色板，但更消耗系统资源。

目前此播放器功能仍较简单，除了按ESC键退出外，不支持主要的播放控制功能，这些功能将在后续更新中推出。

项目中使用了FFMPEG，请到[ffmpeg.org](https://www.ffmpeg.org/)下载Previous FFmpeg 32-bit Dev Versions与Previous FFmpeg 32-bit Shared Versions或Previous FFmpeg 64-bit Dev Versions与Previous FFmpeg 64-bit Shared Versions。然后修改项目属性中的VC++目录和调试中的环境设置为相应目录。