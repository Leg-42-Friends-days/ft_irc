#include "Client.hpp"
#include "Server.hpp"

// ---------------------------------------------- //
// ---------------- CONSTRUCTEURS --------------- //

Client::Client(int fd, const std::string &host)
: _fdClient(fd),
_nickName(""),
_userName(""),
_hostName(host),
_trueName(""),
_inBuf(""),
_hasPwd(false),
_hasNick(false),
_hasUser(false)
{}

// ---------------------------------------------- //
// ------------------- GETTERS ------------------ //

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


// ---------------------------------------------- //
// ------------------- SETTERS ------------------ //

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

void Client::setTrueName(const std::string &trueName)
{
    this->_trueName = trueName;
}


// --------------------------------------------- //
// ----------------- ACCESSEURS ---------------- //

bool Client::hasNick(void) const
{
    return (this->_hasNick);
}

bool Client::hasPwd(void) const
{
    return (this->_hasPwd);
}

bool Client::hasUser(void) const
{
    return (this->_hasUser);
}


// ----------------------------------------- //
// ----------------- BUFFERS ---------------- //

void Client::addInBuf(const std::string &buffer)
{
    this->_inBuf += buffer;
}

bool Client::extractLineInBuf(std::string &line)
{
	std::size_t found = this->_inBuf.find('\n');
    if(found == std::string::npos)
        return false;
    if(found > 0 && (this->_inBuf[found - 1] == '\r'))
       line = this->_inBuf.substr(0, found - 1);
    else
        line = this->_inBuf.substr(0, found);
    this->_inBuf.erase(0, found + 1);
    return true;
}

bool Client::isBufferAboveSize(void) const
{
    if(this->_inBuf.size() > 1024)
        return true;
    return false;
}

// ----------------------------------------- //
// ----------------- OTHERS ---------------- //

std::string Client::prefix(void) const
{
    std::string buildPrefix = this->_nickName + '!' + this->_userName + '@' + this->_hostName;
    return buildPrefix;
}

bool Client::isRegistered(void) const
{
    return(this->_hasPwd && this->_hasNick && this->_hasUser);
}

void Client::validatePassword(void)
{
    this->_hasPwd = true;
}
