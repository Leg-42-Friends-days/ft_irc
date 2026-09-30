#include "Command.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "Server.hpp"
#include "Parser.hpp"
#include "Utils.hpp"

static const CmdInfo cmdInfo[] = {
	{"NICK", cmdNick, 0, false},
	{"PASS", cmdPass, 1, false},
	{"USER", cmdUser, 4, false},
	{"TOPIC", cmdTopic, 1, true},
	{"INVITE", cmdInvite, 2, true},
	{"JOIN", cmdJoin, 1, true},
	// {"KICK", cmdKick, 2, true},
	// {"QUIT", cmdQuit, 0, false}, // parametres optionnels
	{"PRIVMSG", cmdPrivMsg, 0, true}, // 411/412 aucune reponse
	// {"MODE", cmdMode, 1, true},
	// // commande bonus
	// {"NOTICE", cmdNotice, 0, true}, // pour eviter boucle infinie avec le bot, aucune reponse auto
};

std::string nickOrStar(const Client &client);
void assembleResponse(const Client &client, const char * code, const std::string &param, const std::string &text);

void dispatcher(Server &serv, Client &client, const Message &message)
{
    // normaliser verbe en majuscule
    std::string verb = upperCase(message.cmd);
    if(verb.empty())
        return;

    // parcourir table pour touver ligne correspondante
    const CmdInfo * found = NULL;
    size_t cmdCount = (sizeof(cmdInfo) / sizeof(cmdInfo[0]));
    for(size_t i = 0; i < cmdCount; i++)
    {
        if(verb == cmdInfo[i].verb)
        {
            found = &cmdInfo[i];
            break ;
        }
    }
    if (found == NULL)
    {
        assembleResponse(client, ERR_UNKNOWNCOMMAND, verb , "Unknown command");
        return ;
    }

    if(found->needsRegister && !client.isRegistered())
    {
        assembleResponse(client, ERR_NOTREGISTERED, "", "You have not registered");
        return ;
    }

    if(message.params.size() < found->minParams)
    {
        assembleResponse(client, ERR_NEEDMOREPARAMS, verb, "Not enough parameters");
        return ;
    }

    // appel du handler
    found->ft(serv, client, message);
}

void assembleResponse(const Client &client, const char * code, const std::string &param, const std::string &text)
{
	std::ostringstream line;
	line << ':' << SERVER_NAME << " " << code << " " << nickOrStar(client);
	if(!param.empty())
		line <<  " " << param;
	if(!text.empty())
		line <<  " :" << text;
	sendResponse(client, line.str());
}

bool checkFormat(const std::string &msg)
{
	for (size_t i = 0; i < msg.length(); i++)
	{
		if (std::ispunct(msg[i]))
			return (true);
	}
	return (false);
}

void cmdNick(Server &serv, Client &client, const Message &message)
{
	if (message.params.empty())
	{
		assembleResponse(client, ERR_NONICKNAMEGIVEN, "", "Null Nickname isn't a parameter");
		return;
	}

	if (checkFormat(message.params[0]))
	{
		assembleResponse(client, ERR_ERRONEUSNICKNAME, message.params[0], "Special caracter is forbidden");
		return;
	}

	if (checkClientNickName(message.params[0], serv))
	{
		assembleResponse(client, ERR_NICKNAMEINUSE, message.params[0], "Nickname is already in use");
		return;
	}

	client.setNickName(message.params[0]);
	std::cout << "Nickname set to " << message.params[0] << "\n";
}

void cmdPass(Server &serv, Client &client, const Message &message)
{
	if (message.params.empty())
		assembleResponse(client, ERR_PASSWDMISMATCH, "", "Wrong password");
	if (serv.getPassword() != message.params[0])
	{
		assembleResponse(client, ERR_PASSWDMISMATCH, message.params[0], "Wrong password");
		return;
	}
	client.validatePassword();
}

void cmdUser(Server &serv, Client &client, const Message &message)
{
	(void)serv;
	if (message.params.empty())
	{
		assembleResponse(client, ERR_NONICKNAMEGIVEN, "", "Null username isn't a parameter");
		return;
	}

    if (checkFormat(message.params[0]))
	{
		assembleResponse(client, ERR_ERRONEUSNICKNAME, message.params[0], "Special caracter is forbidden");
		return;
	}

	if (checkClientUserName(message.params[0], serv))
	{
		assembleResponse(client, ERR_NICKNAMEINUSE, message.params[0], "Username is already in use");
		return;
	}
    client.setUserName(message.params[0]);
	std::cout << "Username set to " << message.params[0] << "\n";
}

void cmdTopic(Server &serv, Client &client, const Message &message)
{
	Channel * chan = serv.searchChannel(message.params[0]);
    if(chan == NULL)
    {
	    assembleResponse(client, ERR_NOSUCHCHANNEL, message.params[0], "No such channel");
	    return;
    }

	if(!chan->isAMember(&client))
	{
	    assembleResponse(client, ERR_NOTONCHANNEL, chan->getChannelName(), "You're not on that channel");
	    return;
	}

	if(message.params.size() == 1)
	{
		if(chan->getTopic().empty())
		{
        	assembleResponse(client, RPL_NOTOPIC, chan->getChannelName(), "No Topic is set");
			return;
		}
    	else
		{
        	assembleResponse(client, RPL_TOPIC, chan->getChannelName(),chan->getTopic());
			return;
		}
	}

	if(chan->isTopicOpOnly() && !chan->isOperator(&client))
	{
		assembleResponse(client, ERR_CHANOPRIVSNEEDED, chan->getChannelName(), "You're not channel operator");
		return;
	}
	chan->setTopic(message.params[1]);
	std::ostringstream line;
	line << ':' << client.prefix() << " TOPIC " << chan->getChannelName() << " :" << message.params[1];
	chan->broadcast(line.str(), NULL);
}

void cmdInvite(Server &serv, Client &client, const Message &message)
{
	if (!serv.isAClient(message.params[0]))
	{
		assembleResponse(client, ERR_NOSUCHNICK, message.params[0], "No such nick/channel");
		return;
	}
	Channel * chan = serv.searchChannel(message.params[1]);
	if (chan == NULL)
	{
		assembleResponse(client, ERR_NOSUCHCHANNEL, message.params[1], "No such channel");
		return ;
	}
	int error = 0;
	Client  *invited = serv.findClientByNickname(message.params[0])->second;
	error = chan->invite(&client, invited);
	switch (error)
	{
		case 3:
			assembleResponse(client, ERR_USERONCHANNEL, message.params[0] + " " + message.params[1], "is already on channel");
			return;
		case 1:
			assembleResponse(client, ERR_NOTONCHANNEL, message.params[1], "You're not on that channel");
			return;
		case 2:
			assembleResponse(client, ERR_CHANOPRIVSNEEDED, message.params[1], "You're not channel operator");
			return;
		case 0:
			assembleResponse(client, RPL_INVITING, message.params[0] + " " + message.params[1], "");
			sendResponse(*invited, ":" + client.getNickName() + "!" + client.getUserName() + "@localhost INVITE " + message.params[0] + " " + message.params[1]);
			return;
	}
}

void cmdJoin(Server &serv, Client &client, const Message &message)
{
    // Verifier qu'il y a un # devant le nom du channel demande
    if(message.params[0][0] != '#')
    {
        assembleResponse(client, ERR_BADCHANMASK, message.params[0], "Bad Channel Mask");
        return;
    }

    Channel * chan = serv.searchChannel(message.params[0]);
    // Verifier si le channel existe, sinon go le creer et le createur devient operator et membre
    if(chan == NULL)
    {
        chan = serv.addChannel(message.params[0]);
        chan->addOperator(&client);
    }
    else
    {
        // Verifier si channel plein
        if(!chan->isFull())
        {
            assembleResponse(client, ERR_CHANNELISFULL, chan->getChannelName(), "Channel is full");
            return;
        }
        // Verifier si invite-only et non invite
        if(chan->isInviteOnly() && !chan->isInvited(&client))
        {
            assembleResponse(client, ERR_INVITEONLYCHAN, chan->getChannelName(), "Channel is set on invited only");
            return;
        }
        if(chan->isPasswordSet())
        {
            std::string key;
            if(message.params.size() > 1)
                key = message.params[1];
            if(!chan->checkpassword(key))
            {
                assembleResponse(client, ERR_BADCHANNELKEY, chan->getChannelName(), "Cannot join the channel (wrong keys)");
               return;
            }
        }
    }
    // Ajout du membre
    chan->addMember(&client);

    // Retirer invitation si elle existe
    chan->removeInvited(&client);

    // Diffuser :nick!user@host JOIN #channelName a tous les membres du channel
    std::string out = ":" + client.prefix() + " JOIN " + chan->getChannelName();
    chan->broadcast(out, NULL);

    // Topic
    if(chan->getTopic().empty())
         assembleResponse(client, RPL_NOTOPIC, chan->getChannelName(), "No Topic is set");
    else
         assembleResponse(client, RPL_TOPIC, chan->getChannelName(),chan->getTopic());

    // Liste des membres
    std::string param = "= " + chan->getChannelName();
    assembleResponse(client, RPL_NAMREPLY, param, chan->listMembers());
    assembleResponse(client, RPL_ENDOFNAMES, chan->getChannelName(), "End of /NAMES list");
}

void cmdPrivMsg(Server &serv, Client &client, const Message &message)
{
	if(message.params.size() == 0)
	{
		assembleResponse(client, ERR_NORECIPIENT, "", "No recipient given (PRIVMSG)");
		return;
	}
	if(message.params.size() < 2 || message.params[1].empty())
	{
		assembleResponse(client, ERR_NOTEXTTOSEND, message.params[0], "No text to send");
		return;
	}

	if(message.params[0][0] == '#')
	{
		Channel * chan = serv.searchChannel(message.params[0]);
		if (chan == NULL)
		{
			assembleResponse(client, ERR_NOSUCHCHANNEL, message.params[0], "No such channel");
			return ;
		}
		if(!chan->isAMember(&client))
		{
			assembleResponse(client, ERR_CANNOTSENDTOCHAN, message.params[0], "Cannot send to channel");
			return ;
		}
		std::string out = ":" + client.prefix() + " PRIVMSG " + chan->getChannelName() + " :" + message.params[1];
		chan->broadcast(out, &client);
		return;
	}
	Client  *receiver = serv.searchClientByNickname(message.params[0]);
	if (receiver == NULL)
	{
		assembleResponse(client, ERR_NOSUCHNICK, message.params[0], "No such nick/channel");
		return;
	}
	std::string out = ":" + client.prefix() + " PRIVMSG " + receiver->getNickName() + " :" + message.params[1];
	sendResponse(*receiver, out);
	return;
}
// void cmdMode(Server &serv, Client &client, const Message &message)
// {

// }
// void cmdNotice(Server &serv, Client &client, const Message &message)
// {

// }
