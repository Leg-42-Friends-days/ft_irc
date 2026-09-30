#include "Command.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "Server.hpp"
#include "Parser.hpp"

static const CmdInfo cmdInfo[] = {
	{"NICK", cmdNick, 0, false}, // 431 gere par handler
	{"PASS", cmdPass, 1, false},
	{"USER", cmdUser, 4, false},
	//{"TOPIC", cmdTopic, 1, true},
	// {"PING", cmdPing, 0, false}, // 409 gere par handler
	{"INVITE", cmdInvite, 2, true},
	//{"JOIN", cmdJoin, 1, true},
	{"KICK", cmdKick, 2, true},
	{"QUIT", cmdQuit, 0, false}, // parametres optionnels
	// {"PRIVMSG", cmdPrivMsg, 0, true}, // 411/412 aucune reponse
	{"MODE", cmdMode, 1, false},
	// // commande bonus
	// {"LIST", cmdList, 0, true}, // aucun parametre obligatoire
	// {"NOTICE", cmdNotice, 0, true}, // pour eviter boucle infinie avec le bot, aucune reponse auto
};

std::string nickOrStar(const Client &client);
void sendResponse(const Client &client, std::string line);
void assembleResponse(const Client &client, const char *code, const std::string &param, const std::string &text);

void dispatcher(Server &serv, Client &client, const Message &message)
{
	// normaliser verbe en majuscule
	std::string verb = upperCase(message.cmd);
	if (verb.empty())
		return;

	// parcourir table pour touver ligne correspondante
	const CmdInfo *found = NULL;
	size_t cmdCount = (sizeof(cmdInfo) / sizeof(cmdInfo[0]));
	for (size_t i = 0; i < cmdCount; i++)
	{
		if (verb == cmdInfo[i].verb)
		{
			found = &cmdInfo[i];
			break;
		}
	}
	if (found == NULL)
	{
		assembleResponse(client, ERR_UNKNOWNCOMMAND, verb, "Unknown command");
		return;
	}

	if (found->needsRegister && !client.isRegistered())
	{
		assembleResponse(client, ERR_NOTREGISTERED, "", "You have not registered");
		return;
	}

	if (message.params.size() < found->minParams)
	{
		assembleResponse(client, ERR_NEEDMOREPARAMS, verb, "Not enough parameters");
		return;
	}

	// appeler handler
	found->ft(serv, client, message);
}

// static en attendant de savoi où la mettre

void sendResponse(const Client &client, std::string line)
{
	line = line + "\r\n";
	send(client.getFdClient(), line.c_str(), line.size(), 0);
}

void assembleResponse(const Client &client, const char *code, const std::string &param, const std::string &text)
{
	std::ostringstream line;
	line << ':' << SERVER_NAME << " " << code << " " << nickOrStar(client);
	if (!param.empty())
		line << " " << param;
	if (!text.empty())
		line << " :" << text;
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

// void cmdTopic(Server &serv, Client &client, const Message &message)
//{
//  if(!serv.isChannel(message.params[0]))
//  {
//      assembleResponse(client, ERR_NOSUCHCHANNEL, message.cmd, "No such channel");
//      return;
//  }
//  if(!serv.searchChannel(message.params[0]).isAMember(&client))
//  {
//      assembleResponse(client, ERR_NOTONCHANNEL, message.cmd, "You're not on that channel");
//      return;
//  }
//  if(serv.searchChannel(message.params[0]).setTopic(message.params[1], &client))
//  {
//      assembleResponse(client, ERR_CHANOPRIVSNEEDED, message.cmd, "You're not channel operator");
//      return;
//  }
//  else
//  {
//      std::ostringstream line;
//      line << ':' << nickOrStar(client) << " " << message.cmd << " " << message.params[0] << " :" << message.params[1];
//      sendResponse(client, line.str());
//      return;
//  }
//  To be see
//  ERR_NOCHANMODES
//}
// void cmdPing(Server &serv, Client &client, const Message &message)
// {

// }

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
	if (message.params[0][0] == '#')
	{
		assembleResponse(client, ERR_BADCHANMASK, message.params[0], "Bad Channel Mask");
		return;
	}

	Channel *chan = serv.searchChannel(message.params[0]);
	// Verifier si le channel existe, sinon go le creer et le createur devient operator et membre
	if (chan == NULL)
	{
		chan = serv.addChannel(message.params[0]);
		chan->addMember(&client);
		chan->addOperator(&client);
	}
	else
	{
		int result = chan->addMember(&client);
		if (result == 1)
		{
			assembleResponse(client, ERR_CHANNELISFULL, message.params[0], "Channel is full");
			return;
		}
		else if (result == 2)
		{
			assembleResponse(client, ERR_INVITEONLYCHAN, message.params[0], "Channel is set on invited only");
			return;
		}
		// else if()
		//{
		//  Verifier si un mot de passe est set
		//  ERR_BADCHANNELKEY
		//}
		else
		{
		}
	}

	if (chan->getTopic().empty())
		assembleResponse(client, RPL_NOTOPIC, message.params[0], "No Topic is set");
	else
		assembleResponse(client, RPL_TOPIC, message.params[0], chan->getTopic());

	// RPL_NAMREPLY
	// afficher command dans client server
	// RPL_ENDOFNAMES
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
			if (chan->removeMember(user))
			{
				assembleResponse(client, ERR_USERNOTINCHANNEL, client.getNickName() + " " + users[user_i] + " " + channels[chan_i], "They aren't on that channel");
				user_i++;
				continue;
			}
			else
			{
				//brodcast  tous les membres du channel
				//:<nick_de_l_operateur>!<user>@<host> KICK <channel> <target> :<reason>
			}
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
		// fonction pour print les modes;
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
				chan->setInviteOnly(1);
			if (sign == '-')
				chan->setInviteOnly(0);
			break;
		case 't':
			if (sign == '+')
				chan->setTopicChangeOperatorsOnly(1);
			if (sign == '-')
				chan->setTopicChangeOperatorsOnly(0);
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
				i_params++;
			}
			if (sign == '-')
				chan->setPassword("", 0);
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
			}
			if (sign == '-')
			{
				chan->removeOperators(invited);
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
				unsigned int nb = atoi(message.params[i_params].c_str());
				if (nb == 0)
				{
					assembleResponse(client, ERR_INVALIDLIMIT, message.params[0], "Invalid channel limit");
					break;
				}
				i_params++;
				chan->setMaxOfClients(nb, 1);
			}
			if (sign == '-')
				chan->setMaxOfClients(0, 0);
			break;
		default:
			assembleResponse(client, ERR_UNKNOWNMODE, std::string(1, *it), "is unknown mode char to me");
			break;
		}
		it++;
	}
	//RPL_CHANNELMODEIS
	//renvoi les modes actuels d'un channel quand on fait par exemple MODE #42
	// ca revoit par exemple :irc.example.com 324 Bob #42 +nt */
}

// void cmdList(Server &serv, Client &client, const Message &message)
// {

// }
// void cmdNotice(Server &serv, Client &client, const Message &message)
// {

// }
