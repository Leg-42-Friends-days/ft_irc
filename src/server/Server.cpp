#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"

Server::Server(char **av) : _portIP(av[1]) , _password(av[2])
{
}

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
	{
		std::cerr << "Error : can't create a socket" << std::endl;
		throw std::exception();
	}
	
	if (bind(this->_servfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
	{
		std::cerr << "Can't bind to IP/port";
		throw std::exception();
	}
	if (listen(this->_servfd, SOMAXCONN) == -1)
	{
		std::cerr << "Can't listen!" << std::endl;
		throw std::exception();
	}
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
	{
		std::cerr << "Error : accept fonction didn't work";
		throw std::exception();
	}
	Client	newClient(clientFd);
    this->_clientRepertory.push_back(newClient);
	struct pollfd	clientPoll;
	clientPoll.fd = clientFd;
	clientPoll.events = POLLIN;
	clientPoll.revents = 0;
	this->_pollFds.push_back(clientPoll);
	std::cout << "New client : " << clientFd << " (" << inet_ntoa(client_addr.sin_addr) << ")" << std::endl;
	const char* msg = "Welcome to the IRC server !\n";
	send(clientFd, msg, std::strlen(msg), 0);
}

void	Server::receiveMess( struct pollfd &pollFd )
{
	char buffer[4096];
	int message = recv(pollFd.fd, buffer, 4096, 0);
	if (message <= 0)
	{
		std::cout << "Client " << pollFd.fd << " Disconnected" << std::endl;
		close(pollFd.fd);
	}
	else
	{
		buffer[message] = '\0';

		std::cout << "Client " << pollFd.fd << " : " << buffer;

		size_t	j = 0;
		while (j < this->_pollFds.size())
		{
			if (this->_pollFds[j].fd != this->_servfd && this->_pollFds[j].fd != pollFd.fd)
				send(this->_pollFds[j].fd, buffer, message, 0);
			j++;
		}
	}
}