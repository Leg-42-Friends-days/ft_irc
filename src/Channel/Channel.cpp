#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"

Channel::Channel(std::string channelName)
{
	this->_channelName = channelName;
}

void	Channel::addMember( Client *client)
{
	std::cout << "Ajout du membre " << client->getFdClient() << " au serveur " << this->_channelName << std::endl;
	this->_members.insert(std::pair<int, Client*>(client->getFdClient(), client));
}

void	Channel::printChannelMembers( void )
{
	std::map<int, Client*>::iterator	it;
	std::map<int, Client*>::iterator	it_end;
	it = this->_members.begin();
	it_end = this->_members.end();
	std::cout << "Liste des membres du channel " << this->_channelName << " :" << std::endl;
	while(it != it_end)
	{
		std::cout << it->first << std::endl;
		it ++;
	}
}

void	Channel::removeMember( Client *client)
{
	std::cout << "Retrait du membre " << client->getFdClient() << " au serveur " << this->_channelName << std::endl;
	this->_members.insert(std::pair<int, Client*>(client->getFdClient(), client));
}

bool	Channel::isAMember( Client *client)
{
	std::map<int, Client*>::iterator	it;
	std::map<int, Client*>::iterator	it_end;
	it = this->_members.end();
	it = this->_members.find(client->getFdClient());
	if ( it == it_end)
		return (0);
	else
		return (1);
}