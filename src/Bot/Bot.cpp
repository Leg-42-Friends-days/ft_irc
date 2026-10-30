#include "Utils.hpp"
#include "Includes.hpp"


// STRUCTURES //////////////////////////////////////////////////////////////////////////////
struct ElizaRule
{
    const char *    trigger;
    bool    		capture;
    const char *	answer;
};

static const ElizaRule elizaRules[] =
{
	{"suicide", false, "Wake up, YOU ARE ENOUGH. Trust yourself"},
	{"do you love me" , false, "We just met, give us some time"},
	{"do you like me" , false, "Please stay focused"},
	{"i was" , false, "The past is just a story we tell ourselves"},
	{"past" , false, "The past is just a story we tell ourselves"},
	{"last" , false, "It's time to open a new chapter of your life, so just move on"},
	{"help me" , false, "Sanity is not a choice. You can't just choose to get over it"},
	{"depression", false, "Wounds create monsters. And you... You are wounded, Marshal"},
	{"lost", false, "Don't you understand? You're a fu***** rat in a maze"},
	{"stuck", false, "Don't you understand? You're a fu***** rat in a maze"},
	{"mother", false, "I see. Can you tell me more about your family?"},
	{"father", false, "I see. Can you tell me more about your family?"},
	{"family", false, "I see. Can you tell me more about your family?"},
	{"i want to be" , false, "You can be do what we want to do!!!! What's your plan to make it happen?"},
	{"again" , false, "Oups. You did it again..."},
	{"bot", false, "Are you sure you're awake?"},
	{"i need", true, "Why do you need"},
	{"i am", true, "How long have you been"},
	{"i'm" , true, "How long have you been"},
	{"i wish i was" , true, "What's stopping you from becoming"},
	{"i want" , true, "Why do you want"},
	{"unhappy", false, "What goes around comes around. Can you explain what made you unhappy?"},
	{"sad", false, "Who cares ? Oh me, right. Why ?"},
};

// NORMALIZATION //////////////////////////////////////////////////////////////////////////////

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

static std::string addSpaces(std::string str)
{
	return ( ' ' + str + ' ');
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

static std::string normalize (std::string str)
{
	str = toLower(str);
	str = convertPunct(str);
	str = addSpaces(str);
	str = withdrawMultipleSpace(str);
	return str;
}

// CONSTRUCTIONS //////////////////////////////////////////////////////////////////////////////

static const ElizaRule *	findRule(const std::string &sentence, size_t &pos)
{
    size_t ruleCount = (sizeof(elizaRules) / sizeof(elizaRules[0]));
    std::string pattern;
	for(size_t i = 0; i < ruleCount; i++)
    {
		pattern = std::string(" ") + elizaRules[i].trigger + " ";
		std::cerr << "pattern: " << pattern << std::endl;
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
	if(!extract.empty()) // phrase doit etre normaliser et donc avoir un espace a la fin si pas vide
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

std::string defaultAnswer()
{
	std::vector<std::string>  deflt;
	deflt.push_back("Please go on.");
	deflt.push_back("Why do you say that?");
	deflt.push_back("How does that make you feel?");
	deflt.push_back("Can you tell me more about this feeling?");
	deflt.push_back("Can you think about a specific example?");
	deflt.push_back("Ok i understand, and what about it?");
	deflt.push_back("Stop whining, we are here to talk!");
	deflt.push_back("Just stop talking to a fu****** computer and get on with your life.");
	return (deflt[rand() % deflt.size()]);
}

// MAIN FONCTION / ASSEMBLEUR /////////////////////////////////////////////////////////////////
std::string	ElizaAnswer(std::string const &message)
{
	std::cerr << "message: " << message << std::endl;
	std::string messageNormalized = normalize(message);

	std::cerr << "message normaliser: " << messageNormalized << std::endl;

	size_t pos = 0;
	const ElizaRule * rule = findRule(messageNormalized, pos);
	std::cerr << "rule trouver: " << rule << std::endl;
	std::cerr << "pos: " << pos << std::endl;

	std::string answer;
	if(rule == NULL)
	{
		return (defaultAnswer());
	}
	answer = rule->answer;
	if(!rule->capture)
		return answer;
	std::string extract = extractFragment(messageNormalized, pos);
	std::cerr << "extract: "<< extract << std::endl;

	if(extract.empty())
	{
		return (defaultAnswer());
	}
	else
	{
		extract = changePronoun(extract);
		answer = answer + " " + extract + "?";
		return answer;
	}
}
