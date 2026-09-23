#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/Message.hpp"
#include "../../includes/Parser.hpp"
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
	fcntl(this->_servfd, F_SETFL, O_NONBLOCK);
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

void	Server::deleteClient(Client *client)
{
	this->deleteFromAllTheChannels(client);
	std::vector<struct pollfd>::iterator	it;
	it = this->_pollFds.begin();
	while (it != this->_pollFds.end())
	{
		if (it->fd == client->getFdClient())
		{
			this->_pollFds.erase(it);
			break;
		}
		it++;
	}
	close(client->getFdClient());
	if (client->getNickName().empty())
		std::cout << "Client " << client->getFdClient() << " Disconnected" << "\n";
	else
		std::cout << "Client " << client->getNickName() << " Disconnected" << "\n";
	std::map<int, Client*>::iterator	it_client;
	it_client = this->_clientRepertory.find(client->getFdClient());
	if (it_client == this->_clientRepertory.end())
		return ;
	else
		this->_clientRepertory.erase(it_client);
	delete(it_client->second);
}

void	Server::deleteFromAllTheChannels( Client *client)
{
	std::map<std::string, Channel*>::iterator	it;
	it = this->_lobby.begin();
	while (it != this->_lobby.end())
	{
		if (it->second->isAMember(client))
		{
			it->second->removeMember(client);
			if (it->second->isEmpty())
			{
				this->_lobby.erase(it);
				delete(it->second);
			}
		}
		it ++;
	} 
}

void	Server::addChannel( std::string channelName )
{
	Channel	*newChannel = new Channel(channelName);
	this->_lobby.insert(std::pair<std::string, Channel*>(channelName, newChannel));
}


void nickCommand(std::string &buffer, std::map<int, Client*>::iterator it)
{
	std::string cut = cutLine(buffer, 4);
	cut = removeDoubleDot(cut);
	// 	// if (cut.empty())
	// 	// {
	// 	//	insert error no prompt NICK
	// 	// }
	it->second->setNickName(cut);
	std::cout << "nickname set to " << cut << "\n";
}

void userCommand(std::string &buffer, std::map<int, Client*>::iterator it)
{
	std::string cut = cutLine(buffer, 4);
	cut = removeDoubleDot(cut);
	// if (cut.empty())
	// {
	//	insert error no prompt USER
	// }
	it->second->setUserName(cut);
	std::cout << "username set to " << cut << "\n";
}

void passwordCommand(std::string &buffer, std::map<int, Client*>::iterator it)
{
		std::string cut = cutLine(buffer, 3);
		// if (_password == cut)
		// {
			std::cout << "Valid password\n";
			it->second->validatePassword();
		// }
		// else
			// std::cout << "Invalid password\n";
			//what
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

	std::cout << msg.cmd << "\n";

	while (stream >> content)
	{
		msg.params.push_back(content);
		std::cout << content << "\n";
	}

	dispatcher(*this, *client, msg);
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
		this->deleteClient(it->second);
	}
	else
	{
		buffer[message] = '\0';

		std::string inputBuffer = buffer;
		callCommand(inputBuffer, it->second);
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
