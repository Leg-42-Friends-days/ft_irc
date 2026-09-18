#pragma once

#include "Includes.hpp"

// bien penser a donner la fermeture des fd a un destructeur de classe (Serv ou Client)
class Client
{
	private:
		int	_fdClient;
		std::string _nickName; // identite publique du user
		std::string _userName; // utile pour prefixe
		std::string _hostName; // issue de la fonction accept, ne changera jamais, necessaire pour le prefixe
		bool _hasPwd;
		bool _hasNick;
		bool _hasUser;
		Client(void);
		//Client(const Client &other) ;
		Client& operator=(const Client &other);
		
		public:
		// Constructeur
		Client(int fd, const std::string &host);
		
		// Getters
		int	getFdClient(void) const; // mettre une reference sur un int coute plus chere qu'une copie
		const std::string &getNickName(void) const;
		const std::string &getUserName(void) const;
		const std::string &getHostName(void) const;

		// Setters
		void setNickName(const std::string &nickName);
		void setUserName(const std::string &userName);

		// Accesseurs
		bool hasNick() const;
		bool hasPwd() const;
		bool hasUser() const;

		// Others
		std::string prefix() const; // assemblage de nick + user + host
		bool isRegistered() const;
		void validatePassword();
};
