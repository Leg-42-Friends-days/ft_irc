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
		std::string _trueName;
		std::string _inBuf;
		bool _hasPwd;
		bool _hasNick;
		bool _hasUser;
		Client(void);
		Client(const Client &other);
		Client& operator=(const Client &other);

		public:
		// Constructeur
		Client(int fd, const std::string &host);

		// Getters
		int	getFdClient(void) const;
		const std::string &getNickName(void) const;
		const std::string &getUserName(void) const;
		const std::string &getHostName(void) const;

		// Setters
		void setNickName(const std::string &nickName);
		void setUserName(const std::string &userName);
		void setTrueName(const std::string &truename);

		// Accesseurs
		bool hasNick() const;
		bool hasPwd() const;
		bool hasUser() const;

		// Buffers
		void addInBuf(const std::string &buffer);
		bool extractLineInBuf(std::string &line);
		bool isBufferAboveSize() const;

		// Others
		std::string prefix() const; // assemblage de nick + user + host
		bool isRegistered() const;
		void validatePassword();
};
