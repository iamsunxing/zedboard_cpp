#ifndef WEBCAM_H
#define WEBCAM_H

#define VIDEO_DEVICE 	"/dev/video0"
#define BMP      		"image_bmp.bmp"
#define YUV	        	"image_yuv.yuv"
#define JPG	        	"image_jpg.jpg"
//1366   768
//640   480
//必须是16的倍数
#define  IMAGE_WIDTH    640
#define  IMAGE_HEIGHT   480

typedef struct VideoBuffer
{
    void *  start;
    size_t  length;
} VideoBuffer;

extern unsigned char   frame_buffer[IMAGE_WIDTH*IMAGE_HEIGHT*3];


int openVideoDevice();
int getDeviceInfo();
int setCaptureFormat();
int setVideoBuffer();
int startCapture();
int getFrameBuffer();
int putFrameBuffer();
int switch2YuvMap();
int yuv2RGB();
int stopCapture();
int closeVideoDevice();
#endif // WEBCAM_H
