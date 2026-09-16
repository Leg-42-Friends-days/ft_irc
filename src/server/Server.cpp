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

void	Server::addClient(std::vector<struct pollfd> &pollFds)
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
	pollFds.push_back(clientPoll);
	std::cout << "New client : " << clientFd << " (" << inet_ntoa(client_addr.sin_addr) << ")" << std::endl;
	const char* msg = "Welcome to the IRC server !\n";
	send(clientFd, msg, std::strlen(msg), 0);
}