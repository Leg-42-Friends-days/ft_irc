#pragma once

#include "Includes.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Server
{
	private:
		std::string 				_portIP;
		std::string 				_password;
		int							_servfd;
		std::map<int, Client*>		_clientRepertory;
		std::vector<struct pollfd> _pollFds;
		Server(void);
		std::map<Channel, Client> _lobby;

	public:
		Server(char **av);
		const std::string	&getPortIP( void );
		const int	&getServFd( void );
		std::vector<struct pollfd> &getpollFds( void );
		void	initServ( void );
		void	initPollFds( void );
		void	addClient( void );
		void	receiveMess(struct pollfd &pollFd, int index);
		void	callCommand(std::string &buffer, int index);

		class ErrorBindFonction : public std::exception
		{
			public:
				const char *what() const throw();
		};

		class ErrorSocketFonction : public std::exception
		{
			public:
				const char *what() const throw();
		};

		class ErrorListenFonction : public std::exception
		{
			public:
				const char *what() const throw();
		};

		class ErrorAcceptFonction : public std::exception
		{
			public:
				const char *what() const throw();
		};
};
