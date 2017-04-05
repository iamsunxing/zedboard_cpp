
// Definition of the Socket class

#ifndef SOCKET_H
#define SOCKET_H


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
//#include "client_socket.h"

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

#define MAX_SIZE_OF_FRAME (1024*90ul)
typedef struct DataPkg
{
    char pkgh[4];  //x32上  4   x64上 8
    int number; // the number of the frame  4 bytes
    int  size;  // the size of the frame   x32 4   x64 8 bytes
    unsigned char frame[MAX_SIZE_OF_FRAME];
    char pkgt[4];
}__attribute__((__packed__)) DataPkg;


class Socket
{
public:
    Socket();
    virtual ~Socket();

    // Server initialization
    bool create();
    bool bind ( const int port );
    bool listen() const;
    bool accept ( Socket& ) const;

    // Client initialization
    bool connect ( const std::string host, const int port );

    // Data Transimission
    bool send ( const std::string ) const;
    bool send ( const struct DataPkg ) const;
    int recv ( std::string& ) const;


    void set_non_blocking ( const bool );

    bool is_valid() const
    {
        return m_sock != -1;
    }

private:

    int m_sock;
    sockaddr_in m_addr;


};


#endif
