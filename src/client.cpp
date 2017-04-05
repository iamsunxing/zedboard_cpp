#include <iostream>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../header/client_socket.h"
#include "../header/socket_exception.h"

int fsize(FILE * fp)
{
	int num;
	if(fp==NULL) return -1;
    fseek(fp,0,SEEK_END);
    num=ftell(fp);
    rewind(fp);
    return num;
}

DataPkg createpackage(DataPkg & datapkg,char* myjpg)
{
	FILE* fp=fopen(myjpg,"rb");
	if(fp==NULL)
	{
        perror("open jpg failed!\n");
	}
    datapkg.pkgh[0]='p';
    datapkg.pkgh[1]='k';
    datapkg.pkgh[2]='g';
    datapkg.pkgh[3]='h';
    datapkg.number =1;
    datapkg.size=fsize(fp);
    fread(datapkg.frame,1,datapkg.size,fp);
    fclose(fp);
    datapkg.pkgt[0]='p';
    datapkg.pkgt[1]='k';
    datapkg.pkgt[2]='g';
    datapkg.pkgt[3]='t';

    printf("datapkg.size=%d\n",datapkg.size);
    return datapkg;
}
