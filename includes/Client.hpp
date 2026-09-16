#pragma once 

#include "Includes.hpp"

//client herite de l'interface commande
class Client
{
	private:
		int	_fdClient;
		Client( void );
	public:
		Client(int fd);
		const int	&getFdClient( void );
};
