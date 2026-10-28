#include "Utils.hpp"
#include "Includes.hpp"

bool	isNegativeFeeling(const std::string &sentence)
{
	std::vector<std::string>  negatif;
	negatif.push_back("unhappy");
	negatif.push_back("sad");
	negatif.push_back("homeless");
	negatif.push_back("desperate");
	negatif.push_back("hopeless");
	negatif.push_back("hurt");
	std::vector<std::string>::iterator  it;
	it = negatif.begin();
	while (it != negatif.end())
	{
		if (sentence.find(*it) != std::string::npos)
			return true;
		it++;
	}
	return false;
}


std::string	ElizaAnswer(std::string const &message)
{
	std::string messagelow = toLower(message);
	// pontuation par des espaces
	// enlever les espaces multiples
	// rajouter des espaces au debut et a la fin

	if ((messagelow.find("stuck") != std::string::npos) || (messagelow.find("confused") != std::string::npos) || (messagelow.find("lost") != std::string::npos))
		return "Don't you understand? You're a fu***** rat in a maze";

	std::map<std::string, std::string>  answers;

	answers.insert(std::pair<std::string, std::string>("i feel", "Why do you feel "));
	answers.insert(std::pair<std::string, std::string>("i need ", "Why do you need "));
	answers.insert(std::pair<std::string, std::string>("i am ", "How long have you been "));
	answers.insert(std::pair<std::string, std::string>("i'm " , "How long have you been "));
	answers.insert(std::pair<std::string, std::string>("i wish i was " , "What's stopping you from becoming "));
	answers.insert(std::pair<std::string, std::string>("i want " , "What's make you think you want to "));
	answers.insert(std::pair<std::string, std::string>("do you love me ?" , "We just met, give us some time"));
	answers.insert(std::pair<std::string, std::string>("do you like me ?" , "Please stay focused"));

	std::map<std::string, std::string>::iterator    it;
	it = answers.begin();
	while (it != answers.end())
	{
		size_t  pos = messagelow.find(it->first);
		if (pos != std::string::npos)
			return std::string(it->second + message.substr(pos + it->first.size(), message.size()) + "?");
		it ++;
	}
	if (messagelow.find(" mother ") != std::string::npos || messagelow.find(" father ") != std::string::npos)
		return "Tell me more about your family.";

	//desire
	if (messagelow.find(" wish ") != std::string::npos || messagelow.find(" want") != std::string::npos)
		return "You can be do what we want to do!!!! What's your plan to make it happen?";

	//depression
	if (messagelow.find("depressed") != std::string::npos || messagelow.find("depression") != std::string::npos)
		return "Wounds create monsters. And you... You are wounded, Marshal";

	//again
	if (messagelow.find("again") != std::string::npos || messagelow.find("oups") != std::string::npos)
		return "Oups. You did it again...";

	if (isNegativeFeeling(messagelow))
		return "What goes around comes around. Can you explain what made you unhappy?";

	//"Sanity's not a choice, Marshal. You can't just choose to get over it."
	if (messagelow.find("bot") != std::string::npos)
		return "Are you sure you're awake ?";

	if (messagelow.find(" last ") != std::string::npos)
		return "It's time to open a new chapter of your life. Just move on.";

	if ((messagelow.find("i was ") != std::string::npos) || (messagelow.find("past") != std::string::npos))
		return "The past is just a story we tell ourselves.";

	std::vector<std::string>  deflt;
	deflt.push_back("Please go on.");
	deflt.push_back("Why do you say that?");
	deflt.push_back("How does that make you feel?");
	deflt.push_back("Can you tell me more about this feeling?");
	deflt.push_back("Can you think about a specific example?");
	deflt.push_back("Ok i understand, and what about it?");
	deflt.push_back("Stop whining, we are here to talk!");

	return (deflt[rand() % deflt.size()]);
}

// "You are enough."
// "Wake up"
// "I don't care about your depression."
// "Just stop talking to a fu****** computer and get on with your life."
// Is it important for you that
