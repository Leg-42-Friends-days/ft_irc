#include "../includes/Command.hpp"
#include "../includes/Client.hpp"

// void Command::nickcmd(std::string &buffer, std::map<int, Client>::iterator it)
// {

// }

// void Command::executeCommand(std::string &buffer, std::map<int, Client>::iterator it)
// {
// 	std::string contentCmd[3] = {"NICK", "USER", "PWD"};
// 	void (*cmd[3])(std::string &buffer, std::map<int, Client>::iterator it) = {nickCommand, userCommand};

// 	for (int i = 0; i < 3; i++)
// 	{
// 		if (upperCase(first) == contentCmd[i])
// 			cmd[i](line, it);
// 	}
// }   