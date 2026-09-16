#pragma once 

#include "Includes.hpp"
#include "Client.hpp"

class Server
{
	private:
		std::string 		_portIP;
		std::string 		_password;
		int					_servfd;
		std::vector<Client>	_clientRepertory;	
		Server(void);
	public:
		Server(char **av);
		const std::string	&getPortIP( void );
		const int	&getServFd( void );
		void	initServ( void );
		void	addClient(std::vector<struct pollfd> &pollFds);
};