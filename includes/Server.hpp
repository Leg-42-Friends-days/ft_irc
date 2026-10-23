#pragma once

#include "Includes.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Server
{
	private:
		std::string 					_port;
		std::string 					_password;
		int								_servfd;
		std::vector<struct pollfd>		_pollFds;
		std::map<int, Client*>			_clientRepertory;
		std::map<std::string, Channel*>	_lobby;

		Server();

	public:
		// constructeurs
		Server(char **av);
		~Server();

		// getters
		const std::string			&getPort() const;
		int							getServFd() const;
		std::vector<struct pollfd> 	&getPollFds();
		std::map<int, Client*> 		&getClientRepo();
		const std::string 			&getPassword() const;

		// init
		void						initServ();
		void						initPollFds();

		// clients
		void						addClient();
		void						deleteClient(Client *client);
		Client* 					searchClientByNickname(const std::string &nickName);

		// channel
		Channel*					addChannel(const std::string &channelName);
		Channel* 					searchChannel(const std::string &channelName);
		void						deleteFromAllTheChannels(Client *client);
		void						broadcastToMemberInChannels(const Client *client, const std::string &out);

		// others
		void						callCommand(std::string &buffer, Client* client);
		bool						receiveMess(struct pollfd &pollFd);
		bool						sendMess(struct pollfd &pollFd);
		void						updatePollEvents();
};
