#include "Server.hpp"
#include "Parser.hpp"
#include "Utils.hpp"

volatile sig_atomic_t g_running = 1;

void	handler_sig(int)
{
	g_running = 0;
}

void	init_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler_sig;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = handler_sig;
	sigaction(SIGTERM, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);
}

// Boucle principale : attend un event sur les fd surveilles et l'envoi vers accept ou recv, jusqu'a reception d'un signal
void	pollLoop(Server &serv)
{
	while (g_running == 1)
	{
		serv.updatePollEvents();
		int	pollCount = poll(&serv.getPollFds()[0], serv.getPollFds().size(), -1);
		if (pollCount == -1)
		{
			if(errno == EINTR)
				continue;
			else
			{
				std::cerr << "poll error" << std::endl;
				break;
			}
		}

		size_t	i = 0;
		while (i < serv.getPollFds().size())
		{
			if (serv.getPollFds()[i].revents & POLLOUT)
			{
				if(serv.sendMess(serv.getPollFds()[i]))
					continue;
			}

			if (serv.getPollFds()[i].revents & (POLLIN | POLLHUP | POLLERR | POLLNVAL))
			{
				if (serv.getPollFds()[i].fd == serv.getServFd())
				{
					try
					{
						serv.addClient();
					}
					catch(const std::exception& e)
					{
						std::cerr << e.what() << '\n';
						i++;
						continue ;
					}
				}
				else
				{
					try
					{

						if (serv.receiveMess(serv.getPollFds()[i]))
						{
							i++;
							continue;
						}
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
	srand(time(NULL));
	try
	{
		entryParsing(ac, av);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	init_signals();
	Server serv(av);
	try
	{
		serv.initServ();
		std::cerr << "Server is listening to you baby" << std::endl;
		serv.initPollFds();
		pollLoop(serv);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
