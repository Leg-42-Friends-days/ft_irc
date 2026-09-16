#pragma once 

#include "Includes.hpp"

class Server
{
	private:
		std::string _portIP;
		std::string _password;
		Server(void);
	public:
		Server(int ac, char **av);
		const std::string	&getPortIP( void );
};