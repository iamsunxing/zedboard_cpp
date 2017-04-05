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
#include <iostream>
#include "Rgb2Bmp.h"
#include "BMP2JPG.h"
#include "socket.h"
#include "client_socket.h"
#include "client.h"
#include "socket_exception.h"
#include "webcam.h"
using namespace std;
//string server_address = "192.168.1.103";
string server_address = "192.168.1.2";
//const string Server_Address = "127.0.0.1";
int    server_port    = 10001;

DataPkg datapkg;
long nframe=1;
int main(int argc, char**argv)
{
    if(argc == 2)
    {
        if(argv[1][3]=='.')
            server_address=argv[1];
        else
            nframe=atoi(argv[1]);
    }
    if(argc == 3)
    {
        server_address = argv[1];
        int tmp        = atoi(argv[2]);
        if (tmp>=10000)
            server_port=tmp;
        else
            nframe=tmp;
    }
    if(argc == 4)
    {
        server_address = argv[1];
        server_port    = atoi(argv[2]);
        nframe         = atoi(argv[3]);
    }

	if(-1==openVideoDevice())//打开视频设备
	{
		perror("open video failed.");
		return 0;
	}
	getDeviceInfo();//获取设备详细信息
	if(-1==setCaptureFormat())//设置视频捕获格式
	{
		perror("set video capture format failed.");
		return -1;
	}
	if(-1==setVideoBuffer()) //设置视频缓冲区
	{
		perror("set video buffer failed.");
		return -1;
	}
	if(-1==startCapture())  //开始采集视频数据
	{
		perror("");return -1;
	}

    try
    {
        ClientSocket client_socket ( server_address, server_port );
        for(long i=1;i<=nframe;i++)
        {
            if(-1==getFrameBuffer())//获取一帧数据
            {
                perror("get frame buffer failed.");return -1;
            }
            if(-1==switch2YuvMap()) //转换成YUV格式
            {
                perror("switch to YUV failed.");return -1;
            }
            if(-1==yuv2RGB()) //转换成RGB888格式
            {
                perror("YUV to RGB failed.");return -1;
            }

            Rgb2Bmp(frame_buffer,(char*)BMP,IMAGE_WIDTH,IMAGE_HEIGHT);
            printf("save %s OK\n",BMP);

            BMP2JPG((char*)BMP,(char*)JPG);
            createpackage(datapkg,(char*)JPG);
            client_socket << datapkg;
            putFrameBuffer(); //重新放入缓冲区
        }
    }
    catch ( SocketException& e )
    {
        cout << "Exception was caught:" << e.description() << "\n";
    }

	if(-1==stopCapture())  //停止采集视频数据
	{
		perror("");
		return -1;
	}

	closeVideoDevice(); //关闭视频设备
	return 0;
}





