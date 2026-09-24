#pragma once

#include "Includes.hpp"

class Channel
{
	private:
		std::string				_channelName;

		std::map<int, Client*>	_members;
		std::map<int, Client*>	_operators;
		std::map<int, Client*>	_invited;

		std::string				_topic;
		bool					_topicChangeOperatorsOnly;
		bool					_inviteOnly;
		std::string				_password;
		unsigned int			_nbMaxOfClients;
		Channel( void );
	public:
		Channel(std::string channelName);
		bool	isEmpty( void );
		// members
		bool	addMember( Client *client);
		void	removeMember( Client *client);
		void	printChannelMembers( void );
		bool	isAMember( Client *client);
		//operators
		void	addOperator( Client *client);
		void	removeOperators( Client *client);
		void	printChannelOperators( void );
		bool	isOperator( Client *client);

		bool	setTopic(const std::string &topic, Client *Client);
		void	printTopic( void );
		bool	invite(Client *inviter, Client *guest);
		bool	checkpassword(const std::string &password);
		//modes
		void	setTopicChangeOperatorsOnly( bool yesno );
		void	setInviteOnly( bool yesno );
		bool	setPassword(const std::string &password, bool yesno);
		bool	setMaxOfClients(const unsigned int &nb, bool yesno);

		void	sendToAllMembers(const std::string &message);
		void	sendToAllOperators(const std::string &message);
};
