#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <linux/videodev2.h>
//#include <iostream>
#include "Rgb2Bmp.h"
#include "BMP2JPG.h"
#include "webcam.h"
static VideoBuffer * 		buffers;//缓存队列
static struct v4l2_buffer 	buf;//帧缓存
static   		int      	fd; //设备ID，或者说是设备句柄
unsigned char   frame_buffer[IMAGE_WIDTH*IMAGE_HEIGHT*3];
struct v4l2_requestbuffers  req;
size_t frame_length;
void * frame_start;
int    frame_index;
//using namespace std;

//阻塞式打开设备
int openVideoDevice()
{
	 fd = open(VIDEO_DEVICE, O_RDWR);
	 if (fd == -1)
	 {
	 	perror("open video device failed.");
	 	return -1;
	 }
	 return 0;
}
//获取设备的详细信息
int getDeviceInfo()
{
	struct v4l2_capability   cap;//设备功能
	//struct v4l2_format fmt; //格式
	struct v4l2_fmtdesc fmt;
	if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
	{
		perror("get device info failed.");
		return -1;
	}
	else
	{
     	printf("driver:\t\t%s\n",cap.driver);
     	printf("card:\t\t%s\n",cap.card);
     	printf("bus_info:\t%s\n",cap.bus_info);
     	printf("version:\t%d\n",cap.version);
     	printf("capabilities:\t%x\n",cap.capabilities);

     	if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE)
     	{
			printf("Device %s: supports capture.\n",VIDEO_DEVICE);
		}

		if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING)
		{
			printf("Device %s: supports streaming.\n",VIDEO_DEVICE);
		}
	}

	//枚举支持的所有格式
	fmt.index=0;
	fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	printf("Support format:\n");
	while(ioctl(fd,VIDIOC_ENUM_FMT,&fmt)!=-1)
	{
		printf("\t%d.%s\n",fmt.index+1,fmt.description);
		fmt.index++;
	}
	//printf("This is %s,the line number is %d\n",__FILE__,__LINE__);
	return 0;
}

//设置视频捕获格式
int setCaptureFormat()
{
	struct v4l2_format    fmt;
	memset ( &fmt, 0, sizeof(fmt) );
	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = IMAGE_WIDTH;
	fmt.fmt.pix.height      = IMAGE_HEIGHT;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; // 视频数据存储类型，例如是YUV4:2:2还是RGB
	fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
	//printf("This is %s,the line number is %d\n",__FILE__,__LINE__);
	if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
    {
    	perror("set format failed.");
    	return -1;
    }
   // printf("This is %s,the line number is %d\n",__FILE__,__LINE__);
    return 0;
}
//为视频捕捉分配内存
/*

使用VIDIOC_REQBUFS，我们获取了req.count个缓存，
下一步通过调用VIDIOC_QUERYBUF命令来获取这些缓存的地址，
然后使用mmap函数转换成应用程序中的绝对地址，
最后把这段缓存放入缓存队列 buffers

*/
int setVideoBuffer()
{

	unsigned int numBufs=0;
	memset (&req,0,sizeof(req));
	req.count=4;//缓存个数
	req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory=V4L2_MEMORY_MMAP;
	if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) //分配内存
	{
		perror("req bufs failed.");
  		return -1;
	}
	//printf("This is %s,the line number is %d\n",__FILE__,__LINE__);
	buffers = (VideoBuffer*)calloc( req.count, sizeof(*buffers) );// 为缓存队列分配空间
	if(buffers == NULL)
	{
		perror("calloc failed.");
		return -1;
	}
	for (numBufs = 0; numBufs < req.count; numBufs++)
	{
		memset( &buf, 0, sizeof(buf) );
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = numBufs;
		// buf is in the kernel space,buffers in the user space
		if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) //把VIDIOC_REQBUFS中分配的数据缓存转换成物理地址
		{
			perror("VIDIOC_QUERYBUF failed.");
		    return -1;
    	}
	//	printf("This is %s,the line number is %d\n",__FILE__,__LINE__);
		buffers[numBufs].length = buf.length;
		// 转换成相对地址
		buffers[numBufs].start = mmap(	NULL,
										buf.length,
										PROT_READ | PROT_WRITE,
										MAP_SHARED,
										fd,
										buf.m.offset);
		if (buffers[numBufs].start == MAP_FAILED)
		{
			perror("Failed in mmap of capture buffer\n");
		    return -1;
		}
	//	printf("This is %s,the line number is %d\n",__FILE__,__LINE__);
		// 放入缓存队列
		memset (buffers[numBufs].start, 0, buffers[numBufs].length);
		if (ioctl(fd, VIDIOC_QBUF, &buf) == -1)
		{
			perror("VIDIOC_QBUF put into buffer Q failed.");
		    return -1;
		}
	}
	//printf("This is %s,the line number is %d\n",__FILE__,__LINE__);
	return 0;
}
//开始视频流数据的采集
int startCapture()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(-1==ioctl (fd, VIDIOC_STREAMON, &type))
	{
		perror("failed in VIDIOC_STREAMON\n");
		return -1;
	}
	return 0;
}
//获取一帧数据
int getFrameBuffer()
{
	struct v4l2_buffer buf;
	memset(&buf,0,sizeof(buf));
	buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	//printf("This is %s,the line number is %d\n",__FILE__,__LINE__);
	if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1)
	{
		perror("");
		return -1;
	}
	frame_start  = buffers[buf.index].start;
	frame_length = buffers[buf.index].length;
	frame_index  = buf.index;
	//printf("This is %s,the line number is %d\n",__FILE__,__LINE__);

	return 0;
}

int putFrameBuffer()
{
	//重新放入缓存队列
	struct v4l2_buffer buf;
	buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index  = frame_index;
	if (ioctl(fd, VIDIOC_QBUF, &buf) == -1)
	{
		perror("");
		return -1;
	}
	return 0;
}

//把图像数据转换成YUV格式
int switch2YuvMap()
{
	FILE * fyuv = fopen(YUV, "wb");
    if(!fyuv)
	{
		printf("open "YUV" error\n");
		return -1;
	}
	fwrite(frame_start, IMAGE_HEIGHT*IMAGE_WIDTH*2,1, fyuv);
	printf("save " YUV" OK\n");
	fclose(fyuv);
	return 0;
}
int yuv2RGB()
{
	int           	i,j;
    unsigned char 	y1,y2,u,v;
    int 			r1,g1,b1,r2,g2,b2;
    char * 			pointer;


	pointer = (char*)frame_start;

    for(i=0;i<IMAGE_HEIGHT;i++)
    {
    	for(j=0;j<(IMAGE_WIDTH/2);j++)
    	{
    		y1 = *( pointer + (i*(IMAGE_WIDTH/2)+j)*4);
    		u  = *( pointer + (i*(IMAGE_WIDTH/2)+j)*4 + 1);
    		y2 = *( pointer + (i*(IMAGE_WIDTH/2)+j)*4 + 2);
    		v  = *( pointer + (i*(IMAGE_WIDTH/2)+j)*4 + 3);

    		r1 = y1 + 1.042*(v-128);
    		g1 = y1 - 0.34414*(u-128) - 0.71414*(v-128);
    		b1 = y1 + 1.772*(u-128);

    		r2 = y2 + 1.042*(v-128);
    		g2 = y2 - 0.34414*(u-128) - 0.71414*(v-128);
    		b2 = y2 + 1.772*(u-128);

    		if(r1>255)    r1 = 255;
    		else if(r1<0) r1 = 0;

    		if(b1>255)    b1 = 255;
    		else if(b1<0) b1 = 0;

    		if(g1>255)    g1 = 255;
    		else if(g1<0) g1 = 0;

    		if(r2>255)    r2 = 255;
    		else if(r2<0) r2 = 0;

    		if(b2>255)	  b2 = 255;
    		else if(b2<0) b2 = 0;

    		if(g2>255)	  g2 = 255;
    		else if(g2<0) g2 = 0;

    		*(frame_buffer + ((IMAGE_HEIGHT-1-i)*(IMAGE_WIDTH/2)+j)*6    ) = (unsigned char)b1;
    		*(frame_buffer + ((IMAGE_HEIGHT-1-i)*(IMAGE_WIDTH/2)+j)*6 + 1) = (unsigned char)g1;
    		*(frame_buffer + ((IMAGE_HEIGHT-1-i)*(IMAGE_WIDTH/2)+j)*6 + 2) = (unsigned char)r1;
    		*(frame_buffer + ((IMAGE_HEIGHT-1-i)*(IMAGE_WIDTH/2)+j)*6 + 3) = (unsigned char)b2;
    		*(frame_buffer + ((IMAGE_HEIGHT-1-i)*(IMAGE_WIDTH/2)+j)*6 + 4) = (unsigned char)g2;
    		*(frame_buffer + ((IMAGE_HEIGHT-1-i)*(IMAGE_WIDTH/2)+j)*6 + 5) = (unsigned char)r2;
    	}
    }

	return 0;
}

int stopCapture()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(-1==ioctl (fd, VIDIOC_STREAMOFF, &type))
	{
		perror("failed in VIDIOC_STREAMOFF\n");
		return -1;
	}
	return 0;
}

//关闭视频设备
int closeVideoDevice()
{
	unsigned int i=0;
	for(i=0;i<req.count;i++)
	{
		munmap(buffers[i].start,buffers[i].length); //取消内存映射
	}
	if(fd != -1)
	{
		close(fd);
		return 0;
	}
	return -1;
}
