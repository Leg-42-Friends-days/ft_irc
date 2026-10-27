#pragma once

#include "Includes.hpp"

class Client;

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

		Channel();

	public:
		// constructeur
		Channel(const std::string &channelName);

		// channels
		bool				isEmpty() const;
		bool				checkPassword(const std::string &password) const;
		const std::string 	&getChannelName() const;
		bool 				isFull() const;

		// members
		void				addMember(Client *client);
		void				removeMember(const Client &client);
		bool				isAMember(const Client &client) const;
		std::string 		listMembers() const;

		// operators
		void				addOperator(Client *client);
		void				removeOperator(const Client &client);
		bool				isOperator(const Client &client) const;

		// invited
		bool				isInvited(const Client &client) const;
		int					invite(Client *inviter, Client *guest);
		int					addInvite (Client *client );
		void				removeInvited(const Client &client);

		// topic
		void				setTopic(const std::string &topic);
		const std::string	&getTopic() const;
		bool 				isTopicOpOnly() const;

		// modes
		void				setTopicOpOnly(bool yesno);
		void				setInviteOnly(bool yesno);
		bool				isInviteOnly() const;
		void				setPassword(const std::string &password, bool yesno);
		bool				isPasswordSet() const;
		void				setMaxOfClients(const unsigned int &nb);
		std::string 		modesPrinter() const;

		// diffusion
		void				broadcast(const std::string &out, const Client *except) const;
};
