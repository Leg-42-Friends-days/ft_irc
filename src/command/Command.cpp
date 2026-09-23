#include "Command.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "Server.hpp"

static const CmdInfo cmdInfo[] = {
    {"NICK", cmdNick, 0, false}, // 431 gere par handler
    {"PASS", cmdPass, 1, false},
    {"USER", cmdUser, 4, false},
    {"TOPIC", cmdTopic, 1, true},
    {"PING", cmdPing, 0, false}, // 409 gere par handler
    {"INVITE", cmdInvite, 2, true},
    {"JOIN", cmdJoin, 1, true},
    {"KICK", cmdKick, 2, true},
    {"QUIT", cmdQuit, 0, false}, // parametres optionnels
    {"PRIVMSG", cmdPrivMsg, 0, true}, // 411/412 aucune reponse
    {"MODE", cmdMode, 1, true},
    // commande bonus
    {"LIST", cmdList, 0, true}, // aucun parametre obligatoire
    {"NOTICE", cmdNotice, 0, true}, // pour eviter boucle infinie avec le bot, aucune reponse auto
};

static std::string upperCase(std::string buffer);
std::string nickOrStar(const Client &client);
void sendResponse(const Client &client, std::string line);
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

    // appeler handler
    found->ft(serv, client, message);
}

std::string upperCase(std::string buffer)
{
	for (size_t i = 0; i < buffer.length(); i++)
		buffer[i] = toupper(static_cast<unsigned char>(buffer[i]));
	return (buffer);
}

std::string nickOrStar(const Client &client)
{
    if (client.getNickName().empty())
        return "*";
    return client.getNickName();
}

void sendResponse(const Client &client, std::string line)
{
    line = line + "\r\n";
    send(client.getFdClient(), line.c_str(), line.size(), 0);
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

// void nickCommand(std::string &buffer, std::map<int, Client*>::iterator it)
// {
// 	std::string cut = cutLine(buffer, 4);
// 	cut = removeDoubleDot(cut);
// 	// 	// if (cut.empty())
// 	// 	// {
// 	// 	//	insert error no prompt NICK
// 	// 	// }
// 	it->second->setNickName(cut);
// 	std::cout << "nickname set to " << cut << "\n";
// }

// void userCommand(std::string &buffer, std::map<int, Client*>::iterator it)
// {
// 	std::string cut = cutLine(buffer, 4);
// 	cut = removeDoubleDot(cut);
// 	// if (cut.empty())
// 	// {
// 	//	insert error no prompt USER
// 	// }
// 	it->second->setUserName(cut);
// 	std::cout << "username set to " << cut << "\n";
// }

// void passwordCommand(std::string &buffer, std::map<int, Client*>::iterator it)
// {
// 		std::string cut = cutLine(buffer, 3);
// 		// if (_password == cut)
// 		// {
// 			std::cout << "Valid password\n";
// 			it->second->validatePassword();
// 		// }
// 		// else
// 			// std::cout << "Invalid password\n";
// 			//what
// }

void cmdNick(Server &serv, Client &client, const Message &message)
{

}
void cmdPass(Server &serv, Client &client, const Message &message)
{

}
void cmdUser(Server &serv, Client &client, const Message &message)
{

}
void cmdTopic(Server &serv, Client &client, const Message &message)
{

}
void cmdPing(Server &serv, Client &client, const Message &message)
{

}
void cmdInvite(Server &serv, Client &client, const Message &message)
{

}
void cmdJoin(Server &serv, Client &client, const Message &message)
{

}
void cmdKick(Server &serv, Client &client, const Message &message)
{

}
void cmdQuit(Server &serv, Client &client, const Message &message)
{

}
void cmdPrivMsg(Server &serv, Client &client, const Message &message)
{

}
void cmdMode(Server &serv, Client &client, const Message &message)
{

}
void cmdList(Server &serv, Client &client, const Message &message)
{

}
void cmdNotice(Server &serv, Client &client, const Message &message)
{

}
