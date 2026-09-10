#include "../includes/.hpp"


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

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cout << "execute : ./ircserv <port> <password>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string port = av[1];
    std::string password = av[2];

    if (checkPort(av[1]))
    {
        std::cout << "Error : invalid port!" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}