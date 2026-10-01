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
		bool	isEmpty( void ) const;
		bool	checkpassword(const std::string &password);
		const std::string &getChannelName( void ) const;
		bool isFull() const;
		bool isTopicOpOnly() const;
		// members
		void	addMember( Client *client);
		bool	removeMember( Client *client);
		// void	printChannelMembers( void );
		bool	isAMember( Client *client) const;
		std::string listMembers( void ) const;
		//operators
		bool	addOperator( Client *client);
		void	removeOperators( Client *client);
		// void	printChannelOperators( void );
		bool	isOperator( Client *client) const;
		//invited
		bool	isInvited( Client *client) const;
		int		invite(Client *inviter, Client *guest);
		int		addInvite ( Client *client );
		void	removeInvited( Client *client );
		// topic
		void	setTopic(const std::string &topic);
		const	std::string	&getTopic( void );
		//modes
		void	setTopicChangeOperatorsOnly( bool yesno );
		void	setInviteOnly( bool yesno );
		bool	isInviteOnly( void ) const;
		void	setPassword(const std::string &password, bool yesno);
		bool	isPasswordSet( void ) const;
		void	setMaxOfClients(const unsigned int &nb, bool yesno);
		// diffusion
		void	broadcast(const std::string &out, const Client *except) const;
};
