#pragma once

#include "Includes.hpp"

class Channel
{
	private:
		std::string				_channelName;
		std::map<int, Client*>	_members;
		std::map<int, Client*>	_operators;
		std::map<int, Client*>	_invited;
		Channel( void );
	public:
		Channel(std::string channelName);
		void	addMember( Client *client);
		void	printChannelMembers( void );
		void	removeMember( Client *client);
		bool	isAMember( Client *client);
};