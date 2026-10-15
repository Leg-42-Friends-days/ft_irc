#pragma once

const char * const SERVER_NAME 			= "ircserv";

// Reponses enregistrement
const char * const	RPL_WELCOME 			= "001";
const char * const	RPL_YOURHOST 			= "002";
const char * const	RPL_CREATED 			= "003";
const char * const	RPL_MYINFO 				= "004";

// Reponse de commandes
const char * const RPL_CHANNELMODEIS 		= "324";
const char * const RPL_NOTOPIC 				= "331";
const char * const RPL_TOPIC 				= "332";
const char * const RPL_INVITING 			= "341";
const char * const RPL_NAMREPLY 			= "353";
const char * const RPL_ENDOFNAMES 			= "366";

// Erreurs de cible
const char * const 	ERR_NOSUCHNICK 			= "401";
const char * const 	ERR_NOSUCHCHANNEL 		= "403";
const char * const 	ERR_CANNOTSENDTOCHAN 	= "404";
const char * const  ERR_NOORIGIN 			= "409";
const char * const 	ERR_NORECIPIENT 		= "411";
const char * const 	ERR_NOTEXTTOSEND 		= "412";

// Erreur de commande
const char * const 	ERR_UNKNOWNCOMMAND 		= "421";
const char * const 	ERR_NEEDMOREPARAMS 		= "461";

// Erreur de nickname
const char * const 	ERR_NONICKNAMEGIVEN 	= "431";
const char * const 	ERR_ERRONEUSNICKNAME 	= "432";
const char * const 	ERR_NICKNAMEINUSE 		= "433";

// Erreur d'appartenance au canal
const char * const 	ERR_USERNOTINCHANNEL 	= "441";
const char * const 	ERR_NOTONCHANNEL 		= "442";
const char * const 	ERR_USERONCHANNEL 		= "443";

// Erreur d'authentification
const char * const 	ERR_NOTREGISTERED 		= "451";
const char * const 	ERR_ALREADYREGISTRED 	= "462";
const char * const 	ERR_PASSWDMISMATCH 		= "464";
const char * const  ERR_KEYSET 				= "467";

// Erreurs liees aux modes de canal
const char * const 	ERR_CHANNELISFULL 		= "471";
const char * const 	ERR_UNKNOWNMODE 		= "472";
const char * const 	ERR_INVITEONLYCHAN 		= "473";
const char * const 	ERR_BADCHANNELKEY 		= "475";
const char * const 	ERR_BADCHANMASK 		= "476";
const char * const 	ERR_CHANOPRIVSNEEDED 	= "482";
