
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"

Client::Client(int fd) : _fdClient(fd)
{
}

const int	&Client::getFdClient( void )
{
    return (this->_fdClient);
}

