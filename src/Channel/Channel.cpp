#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"

Channel::Channel(std::string channelName) : _topicChangeOperatorsOnly(0)
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
	std::cout << "Retrait du membre " << client->getFdClient() << " au channel " << this->_channelName << std::endl;
	std::map<int, Client*>::iterator	it;
	it = this->_members.find(client->getFdClient());
	this->_members.erase(it);
}

bool	Channel::isAMember( Client *client)
{
	std::map<int, Client*>::iterator	it;
	std::map<int, Client*>::iterator	it_end;
	it_end = this->_members.end();
	it = this->_members.find(client->getFdClient());

	if (it == it_end)
		return (0);
	else
		return (1);
}

void	Channel::addOperator( Client *client)
{
	std::cout << "Ajout de l'operateur " << client->getFdClient() << " au serveur " << this->_channelName << std::endl;
	this->_operators.insert(std::pair<int, Client*>(client->getFdClient(), client));
}
void	Channel::removeOperators( Client *client)
{
	std::cout << "Retrait du membre " << client->getFdClient() << " a la liste des operateurs du channel " << this->_channelName << std::endl;
	std::map<int, Client*>::iterator	it;
	it = this->_operators.find(client->getFdClient());
	this->_operators.erase(it);
}
void	Channel::printChannelOperators( void )
{
	std::map<int, Client*>::iterator	it;
	std::map<int, Client*>::iterator	it_end;
	it = this->_operators.begin();
	it_end = this->_operators.end();
	std::cout << "Liste des operateurs du channel " << this->_channelName << " :" << std::endl;
	while(it != it_end)
	{
		std::cout << it->first << std::endl;
		it ++;
	}
}

bool	Channel::isOperator( Client *client)
{
	std::map<int, Client*>::iterator	it;
	std::map<int, Client*>::iterator	it_end;
	it_end = this->_operators.end();
	it = this->_operators.find(client->getFdClient());

	if (it == it_end)
		return (0);
	else
		return (1);
}

bool	Channel::setTopic(const std::string &topic, Client *Client)
{
	if (topic.empty())
		std::cout << "Topic of the channel" << this->_channelName << " : " << this->_topic << std::endl;
	if (this->_topicChangeOperatorsOnly == 1)
	{
		if (!this->isOperator(Client))
			return (1);
	}
	this->_topic = topic;
	return (0);
}

void	Channel::setTopicChangeOperatorsOnly( bool yesno )
{
	if (yesno == 1)
		this->_topicChangeOperatorsOnly = 1;
	if (yesno == 0)
		this->_topicChangeOperatorsOnly = 0;
}

void	Channel::setInviteOnly( bool yesno )
{
	if (yesno == 1)
		this->_inviteOnly = 1;
	else
		this->_inviteOnly = 0;
}
