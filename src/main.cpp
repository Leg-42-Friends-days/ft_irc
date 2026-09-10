#include "../includes/.hpp"


bool checkPort(std::string port)
{
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
        return (1);
    }
    std::string port = av[1];
    if (checkPort(av[1]))
    {
        std::cout << "Error : invalid port!" << std::endl;
        return (1);
    }
    return EXIT_SUCCESS;
}