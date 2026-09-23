#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/Command.hpp"

// Constructeur
Server::Server(char **av) : _portIP(av[1]) , _password(av[2])
{
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


void	Server::initServ( void )
{
	addrinfo	hint;
	addrinfo	*servinfo;
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;
	int status = getaddrinfo(NULL, this->_portIP.c_str(), &hint, &servinfo);
	(void)status;

	this->_servfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_servfd == -1)
		throw ErrorListenFonction();

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

void	Server::addChannel( std::string channelName )
{
	Channel	*newChannel = new Channel(channelName);
	this->_lobby.insert(std::pair<std::string, Channel>(channelName, *newChannel));
}


void Server::callCommand(std::string &buffer, std::map<int, Client*>::iterator it)
{
	std::string line = trim(buffer);

	while (line[0] == ':')
		line = checkPrefix(line);
	std::stringstream stream(line);

	Message msg;
	std::string content;

	stream >> msg.cmd;

	std::cout << msg.cmd << "\n";

	while (stream >> content)
	{
		msg.params.push_back(content);
		std::cout << content << "\n";
	}

	std::string contentCmd[3] = {"NICK", "USER", "PWD"};
	void (*cmd[3])(std::string &buffer, std::map<int, Client*>::iterator it) = {nickCommand, userCommand};

	for (int i = 0; i < 3; i++)
	{
		if (upperCase(msg.cmd) == contentCmd[i])
			cmd[i](line, it);
	}
	// else if (upperCase(line).compare(0, 4, "JOIN") == 0)
	// {
		// std::string cut = cutLine(line, 4);
	// 	//insert JOIN fonction
	// }
	// else if (upperCase(line).compare(0, 7, "PRIVMSG") == 0)
	// {
		// std::string cut = cutLine(line, 7);
	// 	//insert PRIVMSG fonction
	// }
	// else if (upperCase(line).compare(0, 5, "TOPIC") == 0)
	// {
	// std::string cut = cutLine(line, 5);
	// 	//insert TOPIC fonction
	// else if (upperCase(line).compare(0, 6, "INVITE") == 0)
	// {
		// std::string cut = cutLine(line, 7);
		// insert INVITE fonction
	// }
	// else if (upperCase(line).compare(0, 4, "KICK") == 0)
	// {
	// std::string cut = cutLine(line, 4);
	// 	//insert KICK fonction
	// }
	// }
	// else
	// {
	// 	std::string msg = "command not found : " + line + "\n";
	// 	send(it->first, msg.c_str(), std::strlen(msg.c_str()), 0);
	// }
};

void	Server::receiveMess( struct pollfd &pollFd)
{
	char buffer[4096];
	int message = recv(pollFd.fd, buffer, 4095, 0);
	std::map<int, Client*>::iterator	it;
	it = this->_clientRepertory.find(pollFd.fd);
	if(it == _clientRepertory.end())
		return;
	if (message <= 0)
	{
		if (it->second->getNickName().empty())
			std::cout << "Client " << it->first << " Disconnected" << "\n";
		else
			std::cout << "Client " << it->second->getNickName() << " Disconnected" << "\n";
		close(pollFd.fd);
	}
	else
	{
		buffer[message] = '\0';

		std::string inputBuffer = buffer;
		callCommand(inputBuffer, it);
		std::cout << "BUFFER > " << buffer;
		if (it->second->getNickName().empty())
			std::cout << "Client " << it->second->getFdClient() << "\n";
		else
			std::cout << "Client " << it->second->getNickName() << "\n";
		// size_t	j = 0;
		// while (j < this->_pollFds.size())
		// {
		// 	if (this->_pollFds[j].fd != this->_servfd && this->_pollFds[j].fd != pollFd.fd)
		// 		send(this->_pollFds[j].fd, buffer, message, 0);
		// 	j++;
		// }
	}
}

void	Server::printChannels( void )
{
	std::map<std::string, Channel>::iterator	it;
	std::map<std::string, Channel>::iterator	it_end;
	it = this->_lobby.begin();
	it_end = this->_lobby.end();
	while(it != it_end)
	{
		std::cout << it->first << std::endl;
		it ++;
	}
}

void	Server::deleteFromAllTheChannels( Client *client)
{
	(void)client;
	// parcourir tous les channels, regarder si il est dedans, si il est dedans utiliser delete from the channel
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

const char *Server::ErrorAcceptFonction::what() const throw()
{
	return ("Error : Can't accept!");
}
