#pragma once 

#include "Includes.hpp"

//client herite de l'interface commande
class Client
{
	private:
		int	_fdClient;
		std::string _nickName;
		std::string _userName;
		Client( void );
	public:
		Client(int fd);
		const int	&getFdClient( void );
		const std::string &getNickName(void);
		void changeNickName(std::string &line);
};
