#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/Message.hpp"
#include "../../includes/Parser.hpp"
#include "../../includes/Command.hpp"
#include "../../includes/Utils.hpp"

// Constructeur
Server::Server(char **av) : _portIP(av[1]) , _password(av[2])
{
}

// Destructeur
Server::~Server(void)
{
	// tout destroy, chaque fd, delete chaque client et chaque canal, ferme le socket decoute
	// message : ERROR :Server shutting down
}

// Fonctions GET
const std::string    &Server::getPortIP( void )
{
	return (this->_portIP);
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
	addrinfo	hint;
	addrinfo	*servinfo;
	int			en = 1;
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;
	int status = getaddrinfo(NULL, this->_portIP.c_str(), &hint, &servinfo);
	(void)status;

	this->_servfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_servfd == -1)
		throw ErrorListenFonction();
	if (setsockopt(this->_servfd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw ErrorSetsockoptFonction();

	fcntl(this->_servfd, F_SETFL, O_NONBLOCK);

	if (bind(this->_servfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
		throw std::runtime_error("bind() failed on port " + this->_portIP);

	if (listen(this->_servfd, SOMAXCONN) == -1)
		throw ErrorListenFonction();
}

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
		throw ErrorAcceptFonction();
	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	Client	*newClient = new Client(clientFd, inet_ntoa(client_addr.sin_addr));
	this->_clientRepertory.insert(std::pair<int, Client*>(clientFd, newClient));
	struct pollfd	clientPoll;
	clientPoll.fd = clientFd;
	clientPoll.events = POLLIN;
	clientPoll.revents = 0;
	this->_pollFds.push_back(clientPoll);
	std::cout << "New client : " << clientFd << " (" << newClient->getHostName() << ")" << std::endl;
	const char* msg = "Welcome to the IRC server !\n";
	send(clientFd, msg, std::strlen(msg), 0);

	//test channel
	/* std::map<std::string, Channel>::iterator	it_general;
	it_general = this->_lobby.find("general");
	it_general->second.addMember(newClient);
	it_general->second.printChannelMembers();
	std::cout << "print is a member" << std::endl;
	std::cout << it_general->second.isAMember(newClient) << std::endl;
	it_general->second.removeMember(newClient);
	it_general->second.printChannelMembers();
	std::cout << "print is a member" << std::endl;
	std::cout << it_general->second.isAMember(newClient) << std::endl; */
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
		std::cout << "Client " << client->getFdClient() << " Disconnected" << "\n";
	else
		std::cout << "Client " << client->getNickName() << " Disconnected" << "\n";
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

std::map<int, Client*>::iterator	Server::findClientByNickname( std::string nickname)
{
	std::string normalizeName = toLower(nickname);
	std::map<int, Client*>::iterator	it;
	it = this->_clientRepertory.begin();
	while (it != this->_clientRepertory.end())
	{
		if (it->second->getNickName() == normalizeName)
			return (it);
		it++;
	}
	return (this->_clientRepertory.end());
}

bool	Server::isAClient(std::string nickname)
{
	std::map<int, Client*>::iterator	it;
	it = this->findClientByNickname(nickname);
	if (it == this->_clientRepertory.end())
		return (0);
	else
		return (1);
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

	while (line[0] == ':')
	line = checkPrefix(line);
	std::stringstream stream(line);

	Message msg;
	std::string content;

	stream >> msg.cmd;

	while (stream >> content)
	msg.params.push_back(content);

	dispatcher(*this, *client, msg);
};

bool	Server::receiveMess( struct pollfd &pollFd)
{
	int	fd = pollFd.fd;
	char buffer[4096];
	int message = recv(pollFd.fd, buffer, 4095, 0);
	std::map<int, Client*>::iterator	it;
	it = this->_clientRepertory.find(pollFd.fd);
	if(it == _clientRepertory.end())
		return false;
	if (message <= 0)
	{
		this->deleteClient(it->second);
		return	true;
	}
	else
	{
		buffer[message] = '\0';

		std::string inputBuffer = buffer;
		callCommand(inputBuffer, it->second);
		if (this->_clientRepertory.find(fd) == this->_clientRepertory.end())
			return	true;
		std::cout << "BUFFER > " << buffer;
		return	false;
	}
}

void	Server::printChannels( void )
{
	std::map<std::string, Channel*>::iterator	it;
	std::map<std::string, Channel*>::iterator	it_end;
	it = this->_lobby.begin();
	it_end = this->_lobby.end();
	while(it != it_end)
	{
		std::cout << it->first << std::endl;
		it ++;
	}
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

// Execption

const char *Server::ErrorSocketFonction::what() const throw()
{
	return ("Error : Can't create a socket");
}

const char *Server::ErrorBindFonction::what() const throw()
{
	return ("Error : Can't bind to IP/port");
}

const char *Server::ErrorListenFonction::what() const throw()
{
	return ("Error : Can't listen!");
}

const char *Server::ErrorSetsockoptFonction::what() const throw()
{
	return ("Error : setsockopt() failed");
}

const char *Server::ErrorAcceptFonction::what() const throw()
{
	return ("Error : Can't accept!");
}
