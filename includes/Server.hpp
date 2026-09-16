#pragma once 

#include "Includes.hpp"

class Server
{
	private:
		std::string _portIP;
		std::string _password;
		int			_servfd;
		Server(void);
	public:
		Server(char **av);
		const std::string	&getPortIP( void );
		const int	&getServFd( void );
		void	initServ( void );
};