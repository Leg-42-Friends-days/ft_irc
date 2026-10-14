#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "Parser.hpp"
#include "Command.hpp"
#include "Utils.hpp"

// Constructeur
Server::Server(char **av) : _port(av[1]) , _password(av[2])
{}

// Destructeur
Server::~Server(void)
{
	std::map<int, Client*>::iterator	it_client = this->_clientRepertory.begin();
	std::map<int, Client*>::iterator	it_client_end = this->_clientRepertory.end();

	while (it_client != it_client_end)
	{
		sendResponse(*it_client->second, "ERROR :Server shutting down");
		it_client++;
	}

	it_client = this->_clientRepertory.begin();
	it_client_end = this->_clientRepertory.end();

	while (it_client != it_client_end)
	{
		Client *client = it_client->second;
		close(client->getFdClient());
		delete client;
		it_client++;
	}
	this->_clientRepertory.clear();

	std::map<std::string, Channel*>::iterator	it_chan = this->_lobby.begin();
	std::map<std::string, Channel*>::iterator	it_chan_end = this->_lobby.end();
	while (it_chan != it_chan_end)
	{
		Channel *chan = it_chan->second;
		delete chan;
		it_chan++;
	}
	this->_lobby.clear();

	close(this->_servfd);
	this->_pollFds.clear();
}

// Fonctions GET
const std::string    &Server::getPort( void )
{
	return (this->_port);
}

const int	&Server::getServFd( void )
{
	return (this->_servfd);
}

std::vector<struct pollfd> &Server::getpollFds( void )
{
	return (this->_pollFds);
}

std::map<int, Client*> &Server::getClientRepo( void )
{
	return (this->_clientRepertory);
}

std::string &Server::getPassword( void )
{
	return (this->_password);
}

void	Server::initServ( void )
{
	sockaddr_in local;
	int en = 1;
	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	std::istringstream iss(this->_port);
	int port = 0;
	iss >> port;
	local.sin_port = htons(port);

	this->_servfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_servfd == -1)
		throw std::runtime_error("socket() error during creation");
	if (setsockopt(this->_servfd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw std::runtime_error("setsockopt() error during configuration of the socket");

	if(fcntl(this->_servfd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl() failed");

	if (bind(this->_servfd, (struct sockaddr *)&local, sizeof(local)) == -1)
		throw std::runtime_error("bind() failed on port " + this->_port);

	if (listen(this->_servfd, SOMAXCONN) == -1)
		throw std::runtime_error("listen() error during linking of the socket : " + std::string(strerror(errno)));

}

// void	Server::initServ( void )
// {
// 	addrinfo	hint;
// 	addrinfo	*servinfo;
// 	std::memset(&hint, 0, sizeof(hint));
// 	int			en = 1;
// 	hint.ai_family = AF_INET;
// 	hint.ai_socktype = SOCK_STREAM;
// 	hint.ai_flags = AI_PASSIVE;
// 	int status = getaddrinfo(NULL, this->_port.c_str(), &hint, &servinfo);
// 	(void)status;

// 	this->_servfd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (this->_servfd == -1)
// 		throw std::runtime_error("socket() error during creation");
// 	if (setsockopt(this->_servfd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
// 		throw std::runtime_error("setsockopt() error during configuration of the socket");

// 	if(fcntl(this->_servfd, F_SETFL, O_NONBLOCK) == -1)
// 		throw std::runtime_error("fcntl() failed");

// 	if (bind(this->_servfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
// 		throw std::runtime_error("bind() failed on port " + this->_port);

// 	if (listen(this->_servfd, SOMAXCONN) == -1)
// 		throw std::runtime_error("listen() error during linking of the socket : " + std::string(strerror(errno)));

// 	freeaddrinfo(servinfo);
// }

void	Server::initPollFds( void )
{
	std::vector<struct pollfd> pollFds;
	struct pollfd	servPoll;
	servPoll.fd = this->_servfd;
	servPoll.events = POLLIN;
	servPoll.revents = 0;
	pollFds.push_back(servPoll);
	this->_pollFds = pollFds;
}

void	Server::addClient( void )
{
	sockaddr_in client_addr;
	socklen_t client_size = sizeof(client_addr);
    int clientFd = accept(this->_servfd, (sockaddr *) &client_addr, &client_size);
	if (clientFd == -1)
	{
		std::cerr << "accept() failed" << std::endl;
		return;
	}

	if(fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "fcntl failed" << std::endl;
		close(clientFd);
		return;
	}
	Client	*newClient = new Client(clientFd, inet_ntoa(client_addr.sin_addr));
	this->_clientRepertory.insert(std::pair<int, Client*>(clientFd, newClient));
	struct pollfd	clientPoll;
	clientPoll.fd = clientFd;
	clientPoll.events = POLLIN;
	clientPoll.revents = 0;
	this->_pollFds.push_back(clientPoll);
	std::cerr << "Client " << newClient->getFdClient() << " is connected" << std::endl;
}

void	Server::deleteClient(Client *client)
{
	int	fd = client->getFdClient();
	this->deleteFromAllTheChannels(client);
	std::vector<struct pollfd>::iterator	it;
	it = this->_pollFds.begin();
	while (it != this->_pollFds.end())
	{
		if (it->fd == fd)
		{
			this->_pollFds.erase(it);
			break;
		}
		it++;
	}
	close(fd);
	if (client->getNickName().empty())
		std::cerr << "Client " << client->getFdClient() << " Disconnected" << std::endl;
	else
		std::cerr << "Client " << client->getNickName() << " Disconnected" << std::endl;
	std::map<int, Client*>::iterator	it_client;
	it_client = this->_clientRepertory.find(fd);
	if (it_client == this->_clientRepertory.end())
		return ;
	Client	*toDelete = it_client->second;
	this->_clientRepertory.erase(it_client);
	delete(toDelete);
}
Client* Server::searchClientByNickname(const std::string &nickName)
{
	std::string normalizeName = toLower(nickName);
	std::map<int, Client*>::iterator	it = this->_clientRepertory.begin();
	std::map<int, Client*>::iterator	it_end = this->_clientRepertory.end();

	while (it != it_end)
	{
		if (toLower(it->second->getNickName()) == normalizeName)
			return it->second;
		it++;
	}
	return NULL;
}

void	Server::deleteFromAllTheChannels( Client *client)
{
	std::map<std::string, Channel*>::iterator	it;
	it = this->_lobby.begin();
	while (it != this->_lobby.end())
	{
		Channel	*channel = it->second;
		if (channel->isInvited(client))
			channel->removeInvited(client);
		if (channel->isAMember(client))
		{
			channel->removeMember(client);
			if (channel->isEmpty())
			{
				delete(channel);
				std::map<std::string, Channel*>::iterator	it_temp;
				it_temp = it;
				it ++;
				this->_lobby.erase(it_temp);
			}
			else
				it++;
		}
		else
		it ++;
	}
}

void	Server::broadcastToMemberInChannels(Client *client, const std::string &out)
{
	std::map<std::string, Channel*>::iterator	it;
	std::map<std::string, Channel*>::iterator	it_end;
	it = this->_lobby.begin();
	it_end = this->_lobby.end();
	while (it != it_end)
	{
		Channel	*chan = it->second;
		if (chan->isAMember(client))
			chan->broadcast(out, client);
		it++;
	}
}

void Server::callCommand(std::string &buffer, Client* client)
{

	std::string line = trim(buffer);

	if (line.empty())
		return;
	if (line[0] == ':')
		line = checkPrefix(line);

	Message msg;
	std::string extract;
	size_t space = 0;

	while (!line.empty())
	{
		if(line[0] == ':')
		{
			extract = line.substr(1);
			msg.params.push_back(extract);
			break;
		}
		space = line.find(' ');
		if(space == std::string::npos)
		{
			if(msg.cmd.empty())
			{
				extract = line.substr(0);
				msg.cmd = extract;
				break;
			}
			else
			{
				extract = line.substr(0);
				msg.params.push_back(extract);
				break;
			}
		}
		else
		{
			if(msg.cmd.empty())
			{
				extract = line.substr(0, space);
				msg.cmd = extract;
				line.erase(0, space);
			}
			else
			{
				extract = line.substr(0, space);
				msg.params.push_back(extract);
				line.erase(0, space);
			}
			while (!line.empty() && line[0] == ' ')
				line.erase(0, 1);
		}

	}
	dispatcher(*this, *client, msg);
};

bool	Server::receiveMess( struct pollfd &pollFd)
{
	char buffer[4096];
	int message = recv(pollFd.fd, buffer, 4096, 0);
	std::map<int, Client*>::iterator	it;
	it = this->_clientRepertory.find(pollFd.fd);
	if(it == _clientRepertory.end())
		return false;
	Client *client = it->second;
	int fd = client->getFdClient();
	if (message <= 0)
	{
		this->deleteClient(client);
		return	true;
	}
	else
	{
		std::string s (buffer, message);
		client->addInBuf(s);

	}

	std::string line;
	while(client->extractLineInBuf(line))
	{
		callCommand(line, client);
		if(this->_clientRepertory.find(fd) == this->_clientRepertory.end())
			return true;
	}
	if(client->isBufferAboveSize())
	{
		this->deleteClient(client);
		return	true;
	}
	return false;
}

Channel*	Server::addChannel( const std::string &channelName )
{
	std::string normalizeName = toLower(channelName);
	Channel	*newChannel = new Channel(channelName);
	this->_lobby.insert(std::pair<std::string, Channel*>(normalizeName, newChannel));
	return newChannel;
}

Channel* Server::searchChannel(const std::string &channelName)
{
	std::string normalizeName = toLower(channelName);
	std::map<std::string, Channel*>::iterator	it;
	it = this->_lobby.find(normalizeName);
	if(it == _lobby.end())
		return NULL;
	return it->second;
}
