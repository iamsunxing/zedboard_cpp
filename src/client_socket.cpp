
// Implementation of the ClientSocket class

#include "../header/client_socket.h"

#include "../header/socket_exception.h"


ClientSocket::ClientSocket ( std::string server_addr, int port )
{
    if ( ! Socket::create() )
    {
        throw SocketException ( "Could not create client socket." );
    }

    if ( ! Socket::connect ( server_addr, port ) )
    {
        throw SocketException ( "Could not bind to port." );
    }

}


const ClientSocket& ClientSocket::operator << ( const std::string& s ) const
{
    if ( ! Socket::send ( s ) )
    {
        throw SocketException ( "Could not write to socket." );
    }

    return *this;

}
const ClientSocket& ClientSocket::operator << ( const DataPkg & datapkg ) const
{
    if ( ! Socket::send(datapkg))
    {
        throw SocketException ( "Could not write to socket." );
    }

    return *this;

}

const ClientSocket& ClientSocket::operator >> ( std::string& s ) const
{
    if ( ! Socket::recv ( s ) )
    {
        throw SocketException ( "Could not read from socket." );
    }

    return *this;
}
