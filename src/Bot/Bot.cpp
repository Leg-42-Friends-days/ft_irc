#include "Utils.hpp"
#include "Includes.hpp"

struct ElizaRule
{
    const char *    trigger;
    bool    		capture;
    const char *	answers[3];
};

static const ElizaRule elizaRules[] =
{
	{"unhappy", false, },
	{"sad", false},
	{"homeless", false},
	{"desperate", true},
	{"hopeless", true},
	{"hurt", true},
	{"suicide", true},
	{"kill myself", true},
	{"want to die", true},
	{"end my life", true},
};

static const ElizaRule *	findRule(const std::string &sentence, size_t &pos)
{
    size_t ruleCount = (sizeof(elizaRules) / sizeof(elizaRules[0]));
    std::string pattern;
	for(size_t i = 0; i < ruleCount; i++)
    {
		pattern = std::string(" ") + elizaRules[i].trigger + " ";
        if((pos = sentence.find(pattern)) != std::string::npos)
		{
			pos += pattern.size(); // return pos fin de trigger dans la sentence
            return &elizaRules[i];
		}
    }
	return NULL; // si rien trouver
}


static std::string extractFragment(const std::string &sentence, size_t pos)
{
	std::string extract = sentence.substr(pos);
	if(!extract.empty()) // phrase doit etre normaliser et donc avoir un espace a la fin
		extract.erase(extract.size() - 1);
	return extract;
}

static std::string changePronoun(const std::string &extract)
{
	std::istringstream iss(extract);
	std::string word;
	static const char *  const table[] = {"my", "i", "you", "me", "am", "your", "myself", "i'm", "are"};
	static const char *  const replace[] = {"your", "you", "me", "you", "are", "my", "yourself", "you're", "am"};
	size_t sizeTable = (sizeof(table) / sizeof(table[0]));
	std::string result;
	while(iss >> word)
	{
		bool match = false;
		if(!result.empty())
			result += " ";
		for(size_t i = 0; i < sizeTable; i++)
		{
			if(word == table[i])
			{
				match = true;
				result += replace[i];
				break;
			}
		}
		if(!match)
			result += word;
	}
	return result;
}

// static bool	isNegativeFeeling(const std::string &sentence)
// {
// 	std::vector<std::string>  negatif;
// 	negatif.push_back("unhappy");
// 	negatif.push_back("sad");
// 	negatif.push_back("homeless");
// 	negatif.push_back("desperate");
// 	negatif.push_back("hopeless");
// 	negatif.push_back("hurt");
// 	negatif.push_back("suicide");
// 	negatif.push_back("kill myself");
// 	negatif.push_back("want to die");
// 	negatif.push_back("end my life");
// 	std::vector<std::string>::iterator  it;
// 	it = negatif.begin();
// 	while (it != negatif.end())
// 	{
// 		if (sentence.find(*it) != std::string::npos)
// 			return true;
// 		it++;
// 	}
// 	return false;
// }

static std::string convertPunct(std::string str)
{
	size_t i = 0;
	while(i < str.size())
	{
		if((str[i] != '\'') && (ispunct(static_cast<unsigned char>(str[i]))))
			str[i] = ' ';
		if(isspace(static_cast<unsigned char>(str[i])))
			str[i] = ' ';
		i++;
	}
	return str;
}

static std::string withdrawMultipleSpace(const std::string &str)
{
	std::string result;
	result.reserve(str.size());
	bool space = false;
	for(size_t i = 0; i < str.size(); i++)
	{
		if(str[i] == ' ')
		{
			if(!space)
			{
				space = true;
				result += str[i];
			}
		}
		else
		{
			result += str[i];
			space = false;
		}
	}
	return result;
}

static std::string addSpaces(std::string str)
{
	return ( ' ' + str + ' ');
}

static std::string normalize (std::string str)
{
	str = toLower(str);
	str = convertPunct(str);
	str = addSpaces(str);
	str = withdrawMultipleSpace(str);
	return str;
}


std::string	ElizaAnswer(std::string const &message)
{
	std::string messagelow = normalize(message);

	size_t pos = 0;
	const ElizaRule * rule = findRule(messagelow, pos);

	std::string answer;
	if(rule == NULL)
	{
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

	else // compter reponses non nulles de la regle puis en tirer une au hasard
	{

	}

	if(!rule->capture)
		return answer;
	else
	{
		std::string extract = extractFragment(messagelow);
		if(extract.empty())
			// default answer
		else
			// reflexion puis reponse + " " + fragment + "?"
	}

	// if ((messagelow.find("stuck") != std::string::npos) || (messagelow.find("confused") != std::string::npos) || (messagelow.find("lost") != std::string::npos))
	// 	return "Don't you understand? You're a fu***** rat in a maze";

	// std::map<std::string, std::string>  answers;

	// answers.insert(std::pair<std::string, std::string>("i feel", "Why do you feel "));
	// answers.insert(std::pair<std::string, std::string>("i need ", "Why do you need "));
	// answers.insert(std::pair<std::string, std::string>("i am ", "How long have you been "));
	// answers.insert(std::pair<std::string, std::string>("i'm " , "How long have you been "));
	// answers.insert(std::pair<std::string, std::string>("i wish i was " , "What's stopping you from becoming "));
	// answers.insert(std::pair<std::string, std::string>("i want " , "What's make you think you want to "));
	// answers.insert(std::pair<std::string, std::string>("do you love me ?" , "We just met, give us some time"));
	// answers.insert(std::pair<std::string, std::string>("do you like me ?" , "Please stay focused"));

	// std::map<std::string, std::string>::iterator    it;
	// it = answers.begin();
	// while (it != answers.end())
	// {
	// 	size_t  pos = messagelow.find(it->first);
	// 	if (pos != std::string::npos)
	// 		return std::string(it->second + message.substr(pos + it->first.size(), message.size()) + "?");
	// 	it ++;
	// }
	// if (messagelow.find(" mother ") != std::string::npos || messagelow.find(" father ") != std::string::npos)
	// 	return "Tell me more about your family.";

	// //desire
	// if (messagelow.find(" wish ") != std::string::npos || messagelow.find(" want") != std::string::npos)
	// 	return "You can be do what we want to do!!!! What's your plan to make it happen?";

	// //depression
	// if (messagelow.find("depressed") != std::string::npos || messagelow.find("depression") != std::string::npos)
	// 	return "Wounds create monsters. And you... You are wounded, Marshal";

	// //again
	// if (messagelow.find("again") != std::string::npos || messagelow.find("oups") != std::string::npos)
	// 	return "Oups. You did it again...";

	// // if (isNegativeFeeling(messagelow))
	// // 	return "What goes around comes around. Can you explain what made you unhappy?";

	// //"Sanity's not a choice, Marshal. You can't just choose to get over it."
	// if (messagelow.find("bot") != std::string::npos)
	// 	return "Are you sure you're awake ?";

	// if (messagelow.find(" last ") != std::string::npos)
	// 	return "It's time to open a new chapter of your life. Just move on.";

	// if ((messagelow.find("i was ") != std::string::npos) || (messagelow.find("past") != std::string::npos))
	// 	return "The past is just a story we tell ourselves.";

	// std::vector<std::string>  deflt;
	// deflt.push_back("Please go on.");
	// deflt.push_back("Why do you say that?");
	// deflt.push_back("How does that make you feel?");
	// deflt.push_back("Can you tell me more about this feeling?");
	// deflt.push_back("Can you think about a specific example?");
	// deflt.push_back("Ok i understand, and what about it?");
	// deflt.push_back("Stop whining, we are here to talk!");

	// return (deflt[rand() % deflt.size()]);
}

// "You are enough."
// "Wake up"
// "I don't care about your depression."
// "Just stop talking to a fu****** computer and get on with your life."
// Is it important for you that
