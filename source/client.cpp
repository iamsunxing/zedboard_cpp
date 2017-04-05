
#include "client_socket.h"
#include "socket_exception.h"
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;

//const string Server_Address = "192.168.1.2";
//const int Server_Port=8080;

//DataPkg datapkg;
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
    return datapkg;
}
/*

int main0 ( int argc, char* argv[] )
{
    try
    {
        ClientSocket client_socket ( Server_Address, Server_Port );
      //  string reply;
      //  createpackage();
        try
        {
            cout<<"send start...\n";
           // client_socket << "Test message.";//send message
            client_socket << datapkg;
            cout<<"send OK\n";
          //  client_socket >> reply; //receive message
        }
        catch ( SocketException& ) {}

   //     cout << "We received this response from the server:\n" << reply << "\n";;

    }
    catch ( SocketException& e )
    {
        cout << "Exception was caught:" << e.description() << "\n";
    }

    return 0;
}
*/
