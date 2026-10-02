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
	{"KICK", cmdKick, 2, true},
	// {"QUIT", cmdQuit, 0, false}, // parametres optionnels
	{"PRIVMSG", cmdPrivMsg, 0, true}, // 411/412 aucune reponse
	{"MODE", cmdMode, 1, false},
	// // commande bonus
	{"NOTICE", cmdNotice, 0, true}, // pour eviter boucle infinie avec le bot, aucune reponse auto
};

void assembleResponse(const Client &client, const char * code, const std::string &param, const std::string &text)
{
	std::ostringstream line;
	line << ':' << SERVER_NAME << " " << code << " " << nickOrStar(client);
	if (!param.empty())
		line << " " << param;
	if (!text.empty())
		line << " :" << text;
	sendResponse(client, line.str());
}

void sendWelcome(const Client &client)
{
	assembleResponse(client, RPL_WELCOME, "", "Welcome to IRC Network " + client.prefix());
	assembleResponse(client, RPL_YOURHOST, "", "Your host is Ici Rien ne Crash, running version v1");
	assembleResponse(client, RPL_CREATED, "", "This server was created : 30th september 2026");
	assembleResponse(client, RPL_MYINFO, std::string(SERVER_NAME) + " v1 io itkol", "");
}

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

void cmdNick(Server &serv, Client &client, const Message &message)
{
	bool isAlreadyRegistered = client.isRegistered();
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
	std::string oldPrefix = client.prefix();
	client.setNickName(message.params[0]);
	if(!isAlreadyRegistered && client.isRegistered())
	{
		sendWelcome(client);
		return;
	}
	if(isAlreadyRegistered)
	{
		std::string line = ":" + oldPrefix + " NICK :" + client.getNickName();
		sendResponse(client, line);
		serv.broadcastToMemberInChannels(&client, line);
		return;
	}
}

void cmdPass(Server &serv, Client &client, const Message &message)
{
	if(client.isRegistered())
	{
		assembleResponse(client, ERR_ALREADYREGISTRED, "", "You are already register");
		return;
	}
	if (message.params.empty())
	{
		assembleResponse(client, ERR_PASSWDMISMATCH, "", "Wrong password");
		return;
	}
	if (serv.getPassword() != message.params[0])
	{
		assembleResponse(client, ERR_PASSWDMISMATCH, "", "Wrong password");
		return;
	}
	client.validatePassword();
	if(client.isRegistered())
	{
		sendWelcome(client);
		return;
	}
}

std::string convertToLine(const Message &message)
{
	std::string line;
	for (size_t i = 3; i < message.params.size(); i++)
	{
		line += message.params[i];
		if (i != message.params.size())
			line += " ";
	}

	return (line);
}

bool checkDot(const std::string &buffer)
{
	if (buffer[0] == ':')
		return (true);
	return false;
}

void cmdUser(Server &serv, Client &client, const Message &message)
{

	if (!client.getUserName().empty())
	{
		assembleResponse(client, ERR_ALREADYREGISTRED, "", "already registered");
		return;
	}
	
	// if (checkClientUserName(message.params[0], serv))
	// {
	// 	assembleResponse(client, ERR_NICKNAMEINUSE, message.params[0], "Username is already in use");
	// 	return;
	// }

    client.setUserName(message.params[0]);
	std::cout << "Username set to " << message.params[0] << "\n";

	if (checkDot(message.params[3]))
	{
		std::string line = convertToLine(message);
		line = removeDoubleDot(line);
		client.setTrueName(line);
		std::cout << "True name set to " << line << "\n";
	}
	else
	{
		client.setTrueName(message.params[3]);
		std::cout << "True name set to " << message.params[3] << "\n";
	}
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
	if (message.params[1][0] != '#')
	{
		assembleResponse(client, ERR_BADCHANMASK, message.params[1], "Bad Channel Mask");
		return;
	}
	Channel *chan = serv.searchChannel(message.params[1]);
	if (chan == NULL)
	{
		assembleResponse(client, ERR_NOSUCHCHANNEL, message.params[1], "No such channel");
		return;
	}
	int error = 0;
	Client *invited = serv.findClientByNickname(message.params[0])->second;
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


void cmdKick(Server &serv, Client &client, const Message &message)
{
	std::vector<std::string> channels = splitWithComma(message.params[0]);
	std::vector<std::string> users = splitWithComma(message.params[1]);
	size_t chan_i = 0;
	size_t user_i = 0;
	while (chan_i < channels.size())
	{
		if (channels[chan_i][0] != '#')
		{
			assembleResponse(client, ERR_BADCHANMASK, channels[chan_i], "Bad Channel Mask");
			chan_i++;
			continue;
		}
		Channel *chan = serv.searchChannel(channels[chan_i]);
		if (chan == NULL)
		{
			assembleResponse(client, ERR_NOSUCHCHANNEL, channels[chan_i], "No such channel");
			chan_i++;
			continue;
		}
		if (!chan->isAMember(&client))
		{
			assembleResponse(client, ERR_NOTONCHANNEL, channels[chan_i], "You're not on that channel");
			chan_i++;
			continue;
		}
		if (!chan->isOperator(&client))
		{
			assembleResponse(client, ERR_CHANOPRIVSNEEDED, channels[chan_i], "You're not channel operator");
			chan_i++;
			continue;
		}
		while (user_i < users.size())
		{
			if (!serv.isAClient(users[user_i]))
			{
				assembleResponse(client, ERR_NOSUCHNICK, users[user_i], "No such nick/channel");
				user_i++;
				continue;
			}
			Client *user = serv.findClientByNickname(users[user_i])->second;
			if (chan->isAMember(user))
			{
				assembleResponse(client, ERR_USERNOTINCHANNEL, client.getNickName() + " " + users[user_i] + " " + channels[chan_i], "They aren't on that channel");
				user_i++;
				continue;
			}
			std::string	out;
			if (message.params.size() > 2)
				out = ":" + client.prefix() + " KICK " + chan->getChannelName() + " " + user->getNickName() + " :" + message.params[3] + "\r\n";
			else
				out = ":" + client.prefix() + " KICK " + chan->getChannelName() + " " + user->getNickName() + "\r\n";
   			chan->broadcast(out, NULL);
			chan->removeMember(user);
			user_i++;
		}
		user_i = 0;
		chan_i++;
	}
}

void cmdQuit(Server &serv, Client &client, const Message &message)
{
	(void)serv;
	(void)client;
	(void)message;
	const char *msg = "Aurevoir !\r\n";
	if (message.params.empty())
	{
		send(client.getFdClient(), msg, strlen(msg), 0);
	}
	serv.deleteClient(&client);
}

// void cmdPrivMsg(Server &serv, Client &client, const Message &message)
// {

// }

void cmdMode(Server &serv, Client &client, const Message &message)
{
	if (message.params[0][0] != '#')
	{
		assembleResponse(client, ERR_BADCHANMASK, message.params[0], "Bad Channel Mask");
		return;
	}
	Channel *chan = serv.searchChannel(message.params[0]);
	if (chan == NULL)
	{
		assembleResponse(client, ERR_NOSUCHCHANNEL, message.params[0], "No such channel");
		return;
	}
	if (!chan->isOperator(&client))
	{
		assembleResponse(client, ERR_CHANOPRIVSNEEDED, message.params[0], "You're not channel operator");
		return;
	}
	if (message.params.size() == 1)
	{
		std::string	out = chan->modesPrinter();
		assembleResponse(client, RPL_CHANNELMODEIS, message.params[0], out);
		return;
	}
	std::string mode = message.params[1];
	size_t i_params = 2;
	std::string::iterator it = mode.begin();
	char sign = '\0';
	std::string modes = "itkol";
	while (it != mode.end())
	{
		if (*it == '-' || *it == '+')
		{
			sign = *it;
			it++;
			continue;
		}
		if (sign == '\0')
		{
			assembleResponse(client, ERR_NOSIGN, message.params[1], "Mode sign is missing");
			break;
		}
		switch (*it)
		{
		case 'i':
			if (sign == '+')
			{
				chan->setInviteOnly(1);
				std::string	out = ":" + client.prefix() + " MODE " + chan->getChannelName() + " +i" + "\r\n";
				chan->broadcast(out, NULL);
			}
			if (sign == '-')
			{
				chan->setInviteOnly(0);
				std::string	out = ":" + client.prefix() + " MODE " + chan->getChannelName() + " -i" + "\r\n";
				chan->broadcast(out, NULL);
			}
			break;
		case 't':
			if (sign == '+')
			{
				chan->setTopicChangeOperatorsOnly(1);
				std::string	out = ":" + client.prefix() + " MODE " + chan->getChannelName() + " +t" + "\r\n";
				chan->broadcast(out, NULL);
			}
			if (sign == '-')
			{
				chan->setTopicChangeOperatorsOnly(0);
				std::string	out = ":" + client.prefix() + " MODE " + chan->getChannelName() + " -t" + "\r\n";
				chan->broadcast(out, NULL);
			}
			break;
		case 'k':
		{
			if (sign == '+')
			{
				if (message.params.begin() + i_params == message.params.end() || message.params[i_params] == "")
				{
					assembleResponse(client, ERR_NEEDMOREPARAMS, message.cmd, "Not enough parameters");
					break;
				}
				if (chan->isPasswordSet())
				{
					assembleResponse(client, ERR_KEYSET, message.params[0], "Channel key already set");
					break;
				}
				std::string password = message.params[i_params];
				chan->setPassword(password, 1);
				std::string	out = ":" + client.prefix() + " MODE " + chan->getChannelName() + " +k " + password + "\r\n";
				chan->broadcast(out, NULL);
				i_params++;
			}
			if (sign == '-')
			{
				chan->setPassword("", 0);
				std::string	out = ":" + client.prefix() + " MODE " + chan->getChannelName() + " -k" + "\r\n";
				chan->broadcast(out, NULL);
			}
			break;
		}
		case 'o':
		{
			if (message.params.begin() + i_params == message.params.end() || message.params[i_params] == "")
			{
				assembleResponse(client, ERR_NEEDMOREPARAMS, message.cmd, "Not enough parameters");
				break;
			}
			if (!serv.isAClient(message.params[i_params]))
			{
				assembleResponse(client, ERR_NOSUCHNICK, message.params[i_params], "No such nick/channel");
				break;
			}
			Client *invited = serv.findClientByNickname(message.params[i_params])->second;
			i_params++;
			if (sign == '+')
			{
				if (chan->addOperator(invited))
				{
					assembleResponse(client, ERR_USERNOTINCHANNEL, invited->getNickName() + " " + message.params[0], "They aren't on that channel");
					break;
				}
				std::string	out = ":" + client.prefix() + " MODE " + chan->getChannelName() + " +o " + invited->getNickName() + "\r\n";
				chan->broadcast(out, NULL);
			}
			if (sign == '-')
			{
				chan->removeOperators(invited);
				std::string	out = ":" + client.prefix() + " MODE " + chan->getChannelName() + " -o " + invited->getNickName() + "\r\n";
				chan->broadcast(out, NULL);
			}
			break;
		}
		case 'l':
			if (sign == '+')
			{
				if (message.params.begin() + i_params == message.params.end() || message.params[i_params] == "")
				{
					assembleResponse(client, ERR_NEEDMOREPARAMS, message.cmd, "Not enough parameters");
					break;
				}
				unsigned long nb = std::strtoul(message.params[i_params].c_str(), NULL, 10);
				if (nb == 0 || nb > UINT_MAX || !isOnlyDigits(message.params[i_params]))
				{
					assembleResponse(client, ERR_INVALIDLIMIT, message.params[0], "Invalid channel limit");
					break;
				}
				chan->setMaxOfClients(static_cast<unsigned int>(nb), 1);
				std::string	out = ":" + client.prefix() + " MODE " + chan->getChannelName() + " +l " + message.params[i_params] + "\r\n";
				chan->broadcast(out, NULL);
				i_params++;
			}
			if (sign == '-')
			{
				chan->setMaxOfClients(0, 0);
				std::string	out = ":" + client.prefix() + " MODE " + chan->getChannelName() + " -l" + "\r\n";
				chan->broadcast(out, NULL);
			}
			break;
		default:
			assembleResponse(client, ERR_UNKNOWNMODE, std::string(1, *it), "is unknown mode char to me");
			break;
		}
		it++;
	}
}

// void cmdList(Server &serv, Client &client, const Message &message)
// {

void cmdNotice(Server &serv, Client &client, const Message &message)
{
	if(message.params.size() == 0)
		return;
	if(message.params.size() < 2 || message.params[1].empty())
		return;
	if(message.params[0][0] == '#')
	{
		Channel * chan = serv.searchChannel(message.params[0]);
		if (chan == NULL)
			return ;
		if(!chan->isAMember(&client))
			return ;
		std::string out = ":" + client.prefix() + " NOTICE " + chan->getChannelName() + " :" + message.params[1];
		chan->broadcast(out, &client);
		return;
	}
	Client  *receiver = serv.searchClientByNickname(message.params[0]);
	if (receiver == NULL)
		return;
	std::string out = ":" + client.prefix() + " NOTICE " + receiver->getNickName() + " :" + message.params[1];
	sendResponse(*receiver, out);
	return;
}
