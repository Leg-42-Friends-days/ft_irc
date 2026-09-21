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
		bool					_topic_change_operators_only;
		Channel( void );
	public:
		Channel(std::string channelName);
		// members
		void	addMember( Client *client);
		void	removeMember( Client *client);
		void	printChannelMembers( void );
		bool	isAMember( Client *client);
		//operators
		void	addOperator( Client *client);
		void	removeOperators( Client *client);
		void	printChannelOperators( void );
		bool	isOperator( Client *client);

		bool	setTopic(const std::string &topic, Client *Client);

		//modes
		void	set_topic_change_operators_only( bool yesno );
};