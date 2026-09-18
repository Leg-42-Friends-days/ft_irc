#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"

// Constructeur
Client::Client(int fd, const std::string &host)
: _fdClient(fd),
_nickName(""),
_userName(""),
_hostName(host),
_hasPwd(false),
_hasNick(false),
_hasUser(false)
{}

// Getters
int	Client::getFdClient(void) const
{
    return (this->_fdClient);
}

const std::string &Client::getNickName(void) const
{
    return (this->_nickName);
}

const std::string &Client::getUserName(void) const
{
    return (this->_userName);
}

const std::string &Client::getHostName(void) const
{
    return (this->_hostName);
}

// Setters
void Client::setNickName(const std::string &nickName)
{
    this->_nickName = nickName;
    this->_hasNick = true;
}

void Client::setUserName(const std::string &userName)
{
    this->_userName = userName;
    this->_hasUser = true;
}

// accesseurs
bool Client::hasNick() const
{
    return (this->_hasNick);
}

bool Client::hasPwd() const
{
    return (this->_hasPwd);
}

bool Client::hasUser() const
{
    return (this->_hasUser);
}

// others
std::string Client::prefix() const
{
    std::string buildPrefix = this->_nickName + '!' + this->_userName + '@' + this->_hostName;
    return buildPrefix;
}

bool Client::isRegistered() const
{
    return(this->_hasPwd && this->_hasNick && this->_hasUser);
}

void Client::validatePassword()
{
    this->_hasPwd = true;
}
