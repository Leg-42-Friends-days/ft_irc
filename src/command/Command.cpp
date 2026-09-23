#include "Command.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "Server.hpp"
#include "Parser.hpp"

static const CmdInfo cmdInfo[] = {
    {"NICK", cmdNick, 0, false}, // 431 gere par handler
    // {"PASS", cmdPass, 1, false},
    // {"USER", cmdUser, 4, false},
    {"TOPIC", cmdTopic, 1, true},
    // {"PING", cmdPing, 0, false}, // 409 gere par handler
    // {"INVITE", cmdInvite, 2, true},
    {"JOIN", cmdJoin, 1, true},
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
    (void) serv;
    (void) client;
    (void) message;
    return;
}
// void cmdPass(Server &serv, Client &client, const Message &message)
// {

// }
// void cmdUser(Server &serv, Client &client, const Message &message)
// {

// }

void cmdTopic(Server &serv, Client &client, const Message &message)
{

    if(!serv.isChannel(message.params[0]))
        assembleResponse(client, ERR_NOSUCHCHANNEL, message.cmd, "No such channel");

    if(!serv.searchChannel(message.params[0]).isAMember(&client))
        assembleResponse(client, ERR_NOTONCHANNEL, message.cmd, "You're not on that channel");

    if(serv.searchChannel(message.params[0]).setTopic(message.params[1], &client))
        assembleResponse(client, ERR_CHANOPRIVSNEEDED, message.cmd, "You're not channel operator");
    else
    {
        std::ostringstream line;
        line << ':' << nickOrStar(client) << " " << message.cmd << " " << message.params[0] << " :" << message.params[1];
        sendResponse(client, line.str());
    }
    // To be see
    // ERR_NOCHANMODES
}
// void cmdPing(Server &serv, Client &client, const Message &message)
// {

// }
// void cmdInvite(Server &serv, Client &client, const Message &message)
// {

// }

void cmdJoin(Server &serv, Client &client, const Message &message)
{
    // ERR_BADCHANMASK
    // pas de '#'

    // Channe existe ou pas -> le creer si necessaire

    // ERR_CHANNELISFULL
    // ERR_INVITEONLYCHAN

    // ERR_BADCHANNELKEY

    // RPL_TOPIC

    // pas de topic quqnd tu entres dans un salon
    // 331 RPL_NOTOPIC

    // afficher command dans client server


    // ERR_TOOMANYTARGETS
    // trop darguments ?
}
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
