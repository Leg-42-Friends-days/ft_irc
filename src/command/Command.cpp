#include "Command.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "Server.hpp"
#include "Parser.hpp"

static const CmdInfo cmdInfo[] = {
    {"NICK", cmdNick, 0, false}, // 431 gere par handler
    {"PASS", cmdPass, 1, false},
    // {"USER", cmdUser, 4, false},
    // {"TOPIC", cmdTopic, 1, true},
    // {"PING", cmdPing, 0, false}, // 409 gere par handler
    // {"INVITE", cmdInvite, 2, true},
    // {"JOIN", cmdJoin, 1, true},
    // {"KICK", cmdKick, 2, true},
    // {"QUIT", cmdQuit, 0, false}, // parametres optionnels
    // {"PRIVMSG", cmdPrivMsg, 0, true}, // 411/412 aucune reponse
    // {"MODE", cmdMode, 1, true},
    // // commande bonus
    // {"LIST", cmdList, 0, true}, // aucun parametre obligatoire
    // {"NOTICE", cmdNotice, 0, true}, // pour eviter boucle infinie avec le bot, aucune reponse auto
};

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

// static en attendant de savoi où la mettre

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
// {

// }
// void cmdPing(Server &serv, Client &client, const Message &message)
// {

// }
// void cmdInvite(Server &serv, Client &client, const Message &message)
// {

// }
// void cmdJoin(Server &serv, Client &client, const Message &message)
// {

// }
// void cmdKick(Server &serv, Client &client, const Message &message)
// {

// }
// void cmdQuit(Server &serv, Client &client, const Message &message)
// {

// }
// void cmdPrivMsg(Server &serv, Client &client, const Message &message)
// {

// }
// void cmdMode(Server &serv, Client &client, const Message &message)
// {

// }
// void cmdList(Server &serv, Client &client, const Message &message)
// {

// }
// void cmdNotice(Server &serv, Client &client, const Message &message)
// {
    
// }
