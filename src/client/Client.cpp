
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"

Client::Client(int fd) : _fdClient(fd), _nickName(""), _userName("")
{
}

const int	&Client::getFdClient( void )
{
    return (this->_fdClient);
}

const std::string &Client::getNickName(void)
{
    return (this->_nickName);
}

void Client::changeNickName(std::string &line)
{
    this->_nickName = line;
}
