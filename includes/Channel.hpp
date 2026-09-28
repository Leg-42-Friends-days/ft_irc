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
		int	addMember( Client *client);
		void	removeMember( Client *client);
		void	printChannelMembers( void );
		bool	isAMember( Client *client);
		//operators
		void	addOperator( Client *client);
		void	removeOperators( Client *client);
		void	printChannelOperators( void );
		bool	isOperator( Client *client);
		//invited
		bool	isInvited( Client *client);
		bool	setTopic(const std::string &topic, Client *Client);
		const std::string	&getTopic( void );
		int		invite(Client *inviter, Client *guest);
		bool	checkpassword(const std::string &password);
		//modes
		void	setTopicChangeOperatorsOnly( bool yesno );
		void	setInviteOnly( bool yesno );
		bool	isInviteOnly( void );
		bool	setPassword(const std::string &password, bool yesno);
		bool	isPasswordSet( void );
		bool	setMaxOfClients(const unsigned int &nb, bool yesno);

		void	sendToAllMembers(const std::string &message);
		void	sendToAllOperators(const std::string &message);
};
