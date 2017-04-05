#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
/*
//#define MAX_SIZE_OF_FRAME (640*480*3ul+54)
#define MAX_SIZE_OF_FRAME (1024*90ul)
char *server_ip = "192.168.1.2";    	// 服务器ip地址
unsigned short port = 8080;        	// 服务器的端口号
typedef struct mypkg
{
    char pkgh[4];  //x32上  4   x64上 8
    int number; // the number of the frame  4 bytes
    int  size;  // the size of the frame   x32 4   x64 8 bytes
    unsigned char frame[MAX_SIZE_OF_FRAME];
    char pkgt[4];
}__attribute__((__packed__)) DataPkg;
DataPkg datapkg;
int fsize(FILE * fp)
{
	int num;
    fseek(fp,0,SEEK_END);
    num=ftell(fp);
    rewind(fp);
    return num;
}

void createpackage()
{
	FILE* fp=fopen("1.jpg","rb");
	if(fp==NULL)
	{
        perror("open 1.jpg failed!\n");
	}
    datapkg.pkgh[0]='p';
    datapkg.pkgh[1]='k';
    datapkg.pkgh[2]='g';
    datapkg.pkgh[3]='h';
    datapkg.number=1;
    datapkg.size=fsize(fp);
    fread(datapkg.frame,1,datapkg.size,fp);
    fclose(fp);
    datapkg.pkgt[0]='p';
    datapkg.pkgt[1]='k';
    datapkg.pkgt[2]='g';
    datapkg.pkgt[3]='t';
  //  printf("sizeof(char*)=%d\nsizeof(long)%d\n",sizeof(char*),sizeof(long));
    printf("sizeof(DataPkg)=%d\n",sizeof(DataPkg));
    printf("datapkg.size=%d\n",datapkg.size);

}
int initSendImage()
{
	//if( argc > 1 )		//函数传参，可以更改服务器的ip地址
	{
		server_ip = argv[1];
	}
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);// 创建通信端点：套接字
	if(sockfd < 0)
	{
		perror("socket failed.");
		exit(-1);
	}

	// 设置服务器地址结构体
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr)); // 初始化服务器地址
	server_addr.sin_family = AF_INET;	// IPv4
	server_addr.sin_port = htons(port);	// 端口
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);	// ip

	// 主动连接服务器
	int err_log = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(err_log != 0)
	{
		perror("connect failed.");
		close(sockfd);
		exit(-1);
	}

	createpackage();
	//char send_buf[512] = {0};
	printf("send data to %s:%d\n",server_ip,port);
//	while(1)
	{
		printf("send start...\n");
		//fgets(send_buf,sizeof(send_buf),stdin); // 输入内容
		//send_buf[strlen(send_buf)-1]='\0';
		//send(sockfd, send_buf, strlen(send_buf), 0);   // 向服务器发送信息
		send(sockfd,datapkg.pkgh,12+datapkg.size,0);//发送结构体前四项
		//send(sockfd,datapkg.pkgt,4,0);//发送结构体中最后一项
		printf("send OK\n");
	}

	close(sockfd);

	return 0;
}

*/
