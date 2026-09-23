#pragma once

#include "Includes.hpp"

class Server;
class Client;

#include "Message.hpp"

typedef void (*handler_t)(Server &serv, Client &client, const Message &message);

struct CmdInfo {
    const char * verb;
    handler_t ft;
    size_t minParams;
    bool needsRegister;
};

typedef void (*handler_t)(Server &serv, Client &client, const Message &message);

void dispatcher(Server &serv, Client &client, const Message &message);

void cmdNick(Server &serv, Client &client, const Message &message);
// void cmdPass(Server &serv, Client &client, const Message &message);
// void cmdUser(Server &serv, Client &client, const Message &message);
void cmdTopic(Server &serv, Client &client, const Message &message);
// void cmdPing(Server &serv, Client &client, const Message &message);
// void cmdInvite(Server &serv, Client &client, const Message &message);
// void cmdJoin(Server &serv, Client &client, const Message &message);
// void cmdKick(Server &serv, Client &client, const Message &message);
// void cmdQuit(Server &serv, Client &client, const Message &message);
// void cmdPrivMsg(Server &serv, Client &client, const Message &message);
// void cmdMode(Server &serv, Client &client, const Message &message);
// void cmdList(Server &serv, Client &client, const Message &message);
// void cmdNotice(Server &serv, Client &client, const Message &message);
