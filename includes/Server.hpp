#pragma once

#include "Includes.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Server
{
	private:
		std::string 					_portIP;
		std::string 					_password;
		int								_servfd;
		std::vector<struct pollfd>		_pollFds;
		std::map<int, Client*>			_clientRepertory;
		std::map<std::string, Channel*>	_lobby;
		Server(void);

	public:
		Server(char **av);
		const std::string	&getPortIP( void );
		const int	&getServFd( void );
		std::vector<struct pollfd> &getpollFds( void );
		std::map<int, Client*> &getClientRepo( void );
		std::string &getPassword( void );
		void	initServ( void );
		void	initPollFds( void );
		void	addClient( void );
		void	deleteClient(Client *client);
		void	addChannel( std::string channelName );
		void	receiveMess(struct pollfd &pollFd);
		void	callCommand(std::string &buffer, Client* client);
		//Channel
		void	printChannels( void );
		void	deleteFromAllTheChannels( Client *client);

		//exceptions
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
