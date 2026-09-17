#include "../includes/Server.hpp"

bool checkPort(std::string port)
{
		for (size_t i = 0; i < port.size(); i++)
		{
				if (!isdigit(port[i]))
						return (true);
		}

		int portValue = std::atoi(port.c_str());

		if (portValue < 0 || portValue > 65535)
				return (true);
		return (false);
}

void	init_signals(void)
{
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);
}

void 	entryParsing(int &ac, char **av)
{
	//parsing de l'input du programme 
	if (ac != 3)
		throw std::runtime_error("execute : ./ircserv <port> <password>");
	if (checkPort(av[1]))
		throw std::runtime_error("Error : invalid port!");
}

void	pollLoop(Server &serv)
{
	while (true)
	{
		
		int	pollCount = poll(&serv.getpollFds()[0], serv.getpollFds().size(), -1);
		if (pollCount == -1)
		{
			std::cerr << "poll error" << std::endl;
			break;
		}
		size_t	i = 0;
		while (i < serv.getpollFds().size())
		{
			if (serv.getpollFds()[i].revents & POLLIN)
			{
				if (serv.getpollFds()[i].fd == serv.getServFd())
				{
					try
					{
						serv.addClient();
					}
					catch(const std::exception& e)
					{
						std::cerr << e.what() << '\n';
						continue ;
					}
				}
				else
				{
					try
					{
						serv.receiveMess(serv.getpollFds()[i]);
					}
					catch(const std::exception& e)
					{
						std::cerr << e.what() << '\n';
					}
				}
			}
			i++;
		}
	}
}


int main(int ac, char **av)
{
	try
	{
		entryParsing(ac, av);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return(1);
	}
	Server serv(av);
	//init_signals();
	// CTRL Z pour quitter
	try
	{
		serv.initServ();
		serv.initPollFds();
		pollLoop(serv);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return(1);
	}
	return EXIT_SUCCESS;
}