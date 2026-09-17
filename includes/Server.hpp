#pragma once 

#include "Includes.hpp"
#include "Client.hpp"

class Server
{
	private:
		std::string 				_portIP;
		std::string 				_password;
		int							_servfd;
		std::vector<Client>			_clientRepertory;
		std::vector<struct pollfd> _pollFds;
		Server(void);
	public:
		Server(char **av);
		const std::string	&getPortIP( void );
		const int	&getServFd( void );
		std::vector<struct pollfd> &getpollFds( void );
		void	initServ( void );
		void	initPollFds( void );
		void	addClient( void );
		void	receiveMess(struct pollfd &pollFd);
};