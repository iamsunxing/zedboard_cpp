
// Definition of the ClientSocket class

#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H

#include "../header/socket.h"
class ClientSocket : private Socket
{
public:

    ClientSocket ( std::string server_addr, int port );
    virtual ~ClientSocket() {};

    const ClientSocket& operator << ( const std::string& ) const;
    const ClientSocket& operator << ( const struct DataPkg & datapkg ) const;
    const ClientSocket& operator >> ( std::string& ) const;

};


#endif
