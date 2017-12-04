#include <iostream>
#include <sstream>
#include <cassert>
#include <unordered_set>

unsigned Count(const std::string &input)
{
	unsigned count{0};
	std::istringstream iss(input);
	std::string line;
	while (std::getline(iss, line))
	{
		std::unordered_set<std::string> words;
		std::istringstream is(line);
		std::string word;
		bool isBad{false};
		while (is >> word && !word.empty() && !isBad)
		{
			isBad = !words.insert(word).second;
		}

		if (!isBad)
			++count;
	}
	return count;
}

int main()
{
	assert(Count("aa bb cc dd ee") == 1);
	assert(Count("aa bb cc dd aa") == 0);
	assert(Count("aa bb cc dd aaa") == 1);

	std::string input((std::istreambuf_iterator<char>(std::cin)),
					  std::istreambuf_iterator<char>());
	std::cout << Count(input) << std::endl;
}
