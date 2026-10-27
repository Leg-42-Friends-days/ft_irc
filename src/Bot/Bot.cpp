#include "Command.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "Server.hpp"
#include "Parser.hpp"
#include "Utils.hpp"
#include "Bot.hpp"


std::string	ElizaAnswer(std::string const &message)
{
    std::string messagelow = toLower(message);
    std::map<std::string, std::string>  answers;

    answers.insert(std::pair<std::string, std::string>("i feel ", "Why do you feel "));
    answers.insert(std::pair<std::string, std::string>("i need ", "Why do you need "));
    answers.insert(std::pair<std::string, std::string>("i am ", "How long have you been "));

    std::map<std::string, std::string>::iterator    it;
    it = answers.begin();
    while (it != answers.end())
    {
        size_t  pos = messagelow.find(it->first);
        if (pos != std::string::npos)
            return std::string(it->second + message.substr(pos + it->first.size(), message.size()) + "?");
        it ++;
    }
    if (messagelow.find("mother") != std::string::npos || messagelow.find("father") != std::string::npos)
        return "Tell me more about your family.";

    std::list<std::string>  defau;

    // défaut aléatoire
    static const char *defaults[] = {"Please go on.", "Why do you say that?", "How does that make you feel?"};
    return defaults[rand() % 3];
}