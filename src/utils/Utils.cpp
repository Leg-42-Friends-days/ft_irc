#include "Includes.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "Utils.hpp"

// Fonction qui ajoute \r\n a a fin d'une reponse
void sendResponse(const Client &client, std::string line)
{
    line = line + "\r\n";
    send(client.getFdClient(), line.c_str(), line.size(), 0);
}

// Fonction qui passe tous les characteres d'une string en minuscule
std::string toLower(std::string str)
{
	size_t i = 0;
	while(i < str.size())
	{
		str[i] = tolower(static_cast<unsigned char>(str[i]));
		i++;
	}
	return str;
}

// Fonction qui verifie si une string contient uniquement des digits
bool	isOnlyDigits(const std::string &str)
{
	std::string::const_iterator	it = str.begin();
	while (it != str.end())
	{
		if (!std::isdigit(static_cast<unsigned char>(*it)))
			return (0);
		it++;
	}
	return (1);
}

// Fonction qui assemble la struct message en une seule ligne
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

// Fonction qui verifie si une string commence par ':'
bool checkDot(const std::string &buffer)
{
	if (buffer[0] == ':')
		return (true);
	return false;
}
