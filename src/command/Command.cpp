#include "Command.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "Server.hpp"
#include "Parser.hpp"

static const CmdInfo cmdInfo[] = {
    {"NICK", cmdNick, 0, false}, // 431 gere par handler
    {"PASS", cmdPass, 1, false},
    {"USER", cmdUser, 4, false},
    {"TOPIC", cmdTopic, 1, true},
    // {"PING", cmdPing, 0, false}, // 409 gere par handler
    //{"INVITE", cmdInvite, 2, true},
    //{"JOIN", cmdJoin, 1, true},
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
    (void)serv;
    if (message.params.empty())
    {
        assembleResponse(client, ERR_NONICKNAMEGIVEN, "", "Null username isn't a parameter");
        return;
    }

}

void cmdTopic(Server &serv, Client &client, const Message &message)
{

    if(!serv.isChannel(message.params[0]))
    {
        assembleResponse(client, ERR_NOSUCHCHANNEL, message.cmd, "No such channel");
        return;
    }
    if(!serv.searchChannel(message.params[0]).isAMember(&client))
    {
        assembleResponse(client, ERR_NOTONCHANNEL, message.cmd, "You're not on that channel");
        return;
    }
    if(serv.searchChannel(message.params[0]).setTopic(message.params[1], &client))
    {
        assembleResponse(client, ERR_CHANOPRIVSNEEDED, message.cmd, "You're not channel operator");
        return;
    }
    else
    {
        std::ostringstream line;
        line << ':' << nickOrStar(client) << " " << message.cmd << " " << message.params[0] << " :" << message.params[1];
        sendResponse(client, line.str());
        return;
    }
    // To be see
    // ERR_NOCHANMODES
}
// void cmdPing(Server &serv, Client &client, const Message &message)
// {

// }
//void cmdInvite(Server &serv, Client &client, const Message &message)
//{
    
    //ERR_NOSUCHNICK
    //le mec n'existe pas

    //ERR_NOTONCHANNEL
    //l'inviteur n'appartient pas au channel

void cmdJoin(Server &serv, Client &client, const Message &message)
{
    // Verifier qu'il y a un # devant le nom du channel demande
    if(!strncmp(message.params[0].c_str(), "#", 1))
    {
        assembleResponse(client, ERR_BADCHANMASK, message.params[0], "Bad Channel Mask");
        return;
    }
    // Verifier si le channel existe, sinon go le creer
    if(!serv.isChannel(message.params[0]))
        serv.addChannel(message.params[0]);

    // Verifier si le channel est en invite only
        // ERR_INVITEONLYCHAN
    // Verifier si un mot de passe est set
        // ERR_BADCHANNELKEY

    // verifier si le channel est full, sinon ajouter le membre
    int result = serv.searchChannel(message.params[0]).addMember(&client);
    if(result == 1)
    {
        assembleResponse(client, ERR_CHANNELISFULL, message.params[0], "Channel is full");
        return;
    }else if(result == 2)
    {
        assembleResponse(client, ERR_INVITEONLYCHAN, message.params[0], "Channel is set on invited only");
        return;
    }
    else
    {
        // print aux operators que x a rejoint le channel ?
    }
    if(serv.searchChannel(message.params[0]).getTopic().c_str() == NULL)
         assembleResponse(client, RPL_NOTOPIC, message.params[0], "No Topic is set");
    else
         assembleResponse(client, RPL_TOPIC, message.params[0], serv.searchChannel(message.params[0]).getTopic());

    // RPL_NAMREPLY
    // afficher command dans client server
    // RPL_ENDOFNAMES
    // ERR_TOOMANYTARGETS
    // trop darguments ?
////}
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
