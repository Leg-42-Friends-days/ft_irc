#pragma once

#include "Includes.hpp"

class Client
{
	private:
		int			_fdClient;
		std::string	_nickName; // identite publique du user
		std::string _userName; // utile pour prefixe
		std::string _hostName; // issue de la fonction accept, utile pour prefixe
		std::string _trueName; // quatrieme parametre de USER (aussi appeler realname)
		std::string _inBuf;
		std::string _outBuf;
		bool 		_hasPwd;
		bool 		_hasNick;
		bool 		_hasUser;

		Client();
		Client(const Client &other);
		Client& operator=(const Client &other);

	public:
		// constructeur
		Client(int fd, const std::string &host);

		// getters
		int					getFdClient() const;
		const std::string 	&getNickName() const;
		const std::string 	&getUserName() const;
		const std::string 	&getHostName() const;

		// setters
		void 				setNickName(const std::string &nickName);
		void 				setUserName(const std::string &userName);
		void 				setTrueName(const std::string &truename);

		// accesseurs
		bool 				hasNick() const;
		bool 				hasPwd() const;
		bool 				hasUser() const;

		// buffers
		void 				addInBuf(const std::string &buffer);
		bool 				extractLineInBuf(std::string &line);
		bool 				isBufferAboveSize() const;

		void				addOutBuf(const std::string &data);
		bool 				hasOutput() const;
		const std::string 	&getOutBuf() const;
		void 				consumeOutBuf(size_t n);
		bool 				isOutBufAboveSize() const;

		// others
		std::string 		prefix() const; // assemblage de nick + user + host
		bool 				isRegistered() const;
		void 				validatePassword();
};
