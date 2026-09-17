#pragma once 

#include "Includes.hpp"

//client herite de l'interface commande
class Client
{
	private:
		int	_fdClient;
		Client( void );
		std::string _nickName;
		std::string _userName;
	public:
		Client(int fd);
		const int	&getFdClient( void );
		// const std::string &getUserName(void);
};
