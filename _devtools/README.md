*This project has been created as part of the 42 curriculum by <ibrouin->, <jlemoine>, <mickzhan>.*

**Description**

**Instruction**

**Ressources**

**Commandes rapides**

*Lancement serveur*
./ircserv <port> <pwd>

*Connexion d'un client*
nc 127.0.0.1 (ou localhost) <port>

*Verifier connexions actives sur le Port*
lsof -i :<port>

*Forcer la Fermeture des Processus Actifs sur le Port <port>*
kill -9 $(lsof -t -i :<port>)

*Verifier fuites memoires et fd actifs*
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes ./ircserv <port> <password>

*Enregistrement d'un client*
- NICK <nickname>
-> Mettre simplement le surnom/nom du client
=> "NICK Julio"

- PWD <password>
-> Entrer le mot de passe du serveur

- USER (doit respecter un certain nombre de regles)
=> "USER ju 0 * :Julio"
		 ^^ ^ ^  ^^^^^^^
		 |  | |  *realname* (trailing, libre, espaces autorisés)
		 |  | *unused* — historiquement un hostname, ignoré, mets *
		 |  *mode* — bitmask RFC 2812, personne ne l'utilise, mets 0
		 *username* — celui du préfixe nick!user@host

- JOIN <channel>
-> Si le channel n'existe pas, le creer et en devenir operator
-> Si le channel existe, le rejoindre en simple membre

- PRIVMSG <message target> :<message>
-> message target  peut aussi bien etre un channel qu'un user (commence par un # si c'est un channel)
-> le serveur renvoie : "julio!ju@127.0.0.1 PRIVMSG #dev :salut" (ajout du prefixe source, le txt est jamais touche)
-> attention le message ne doit pas etre renvoyer a son auteur
-> si erreur (canal inexistant ou introuvable, pas de texte), il ne faut pas renvoyer de message derreur, il faut ignorer silencieusement


**Tests parsing commandes**
PRIVMSG #dev :salut à tous
JOIN #dev
KICK #dev bob :flood repete
MODE #dev +o bob
TOPIC #dev
TOPIC #dev :
NICK
PRIVMSG
:julio!ju@host PRIVMSG #dev :coucou
PRIVMSG #dev ::double deux points
JOIN    #dev

**Methode de parsing**
1. si sa commence par ':' -> c'est un [prefixe], go ignorer/skip on en a pas besoin
	si sa commence pas par ':', extraire la [command] jusquau prochain espace
2. boucler sur creation de token [params] tant quil y a des espaces entre chaque, sauf si on trouve un ':' a ce moment il faut recuperer tout ce quil y a derriere dans un seul bloc (et skip le ':', on ne le garde pas dans les params envoyees).
* si le parametre est vide apres un ':', bien mettre une chaine nulle/vide c'est important pour la suite (exemple "TOPIC #dev :" signifie supprime le topic, donc si ':' sans rien derriere il faut quand meme lenregistrer pour le detecter)

* handler cmd TOPIC doit verifier existence du canal (sinon 403), si lauteur en est membre (sinon 402), si t+ est actif (necessaire pour ecrire et non juste consulter) et lauteur non operateur -> renvoyer 482

**Deux types de trames sortantes**
- les numeriques -> pour le serveur qui repond a un client
- les relais -> pour le serveur qui notifie un groupe qu'un utilisateur a fait quelque chose, pas de code numerique ici mais on ajoute le prefixe de l'auteur de l'action notifier ([prefixe source] [cmd] [canal] [trailing] [\r\n]), autrement dit la trame sortante cest la trame entrante avec un prefixe devant
* Diffusion : PRIVMSG et NOTICE excluent l'auteur. JOIN,PART, QUIT, KICK, TOPIC, MODE incluent l'auteur.
* Ordre : valider -> construire trame -> diffuser -> modifier l'etat

*NUMERIQUES UTILES*

// Reponses enregistrement
001	RPL_WELCOME
002	RPL_YOURHOST
003	RPL_CREATED
004 RPL_MYINFO

// Reponse de commandes
324	RPL_CHANNELMODEIS
331	RPL_NOTOPIC (le canal n'a pas de topic)
332	RPL_TOPIC (le canal a un topic)
341	RPL_INVITING
353	RPL_NAMREPLY
366	RPL_ENDOFNAMES

// Erreurs de cible
401	ERR_NOSUCHNICK
403	ERR_NOSUCHCHANNEL
404	ERR_CANNOTSENDTOCHAN
411	ERR_NORECIPIENT
412	ERR_NOTEXTTOSEND

// Erreur de commande
421	ERR_UNKNOWNCOMMAND
461	ERR_NEEDMOREPARAMS

// Erreur de nickname
431	ERR_NONICKNAMEGIVEN
432	ERR_ERRONEUSNICKNAME
433	ERR_NICKNAMEINUSE

// Erreur d'appartenance au canal (Attention a ne pas confondre les trois)
441	ERR_USERNOTINCHANNEL
442	ERR_NOTONCHANNEL
443	ERR_USERONCHANNEL

// Erreur d'authentification
451	ERR_NOTREGISTERED
462	ERR_ALREADYREGISTRED
464	ERR_PASSWDMISMATCH

// Erreurs liees aux modes de canal
471	ERR_CHANNELISFULL
472	ERR_UNKNOWNMODE
473	ERR_INVITEONLYCHAN
475	ERR_BADCHANNELKEY
476	ERR_BADCHANMASK (pas de #, caracteres interdits, peut renvoyer 403 a la place)
482	ERR_CHANOPRIVSNEEDED

