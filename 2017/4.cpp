#include <doctest/doctest.h>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

template <typename FilterT>
unsigned Count(const std::string &input)
{
	unsigned count{0};
	std::istringstream iss(input);
	std::string line;
	while (std::getline(iss, line))
	{
		FilterT filter;
		std::istringstream is(line);
		std::string word;
		bool isBad{false};
		while (is >> word && !word.empty() && !isBad)
		{
			isBad = filter.Add(word);
		}

		if (!isBad)
			++count;
	}
	return count;
}

struct ExactFilter
{
	bool Add(const std::string &word)
	{
		return !_words.insert(word).second;
	}

private:
	std::unordered_set<std::string> _words;
};

struct AnagramFilter
{
	bool Add(std::string word)
	{
		std::sort(word.begin(), word.end());
		return !_words.insert(word).second;
	}

private:
	std::unordered_set<std::string> _words;
};

TEST_CASE(TEST_NAME)
{
	REQUIRE(Count<ExactFilter>("aa bb cc dd ee") == 1);
	REQUIRE(Count<ExactFilter>("aa bb cc dd aa") == 0);
	REQUIRE(Count<ExactFilter>("aa bb cc dd aaa") == 1);

	std::ifstream ifs(INPUT);
	std::string input((std::istreambuf_iterator<char>(ifs)),
					  std::istreambuf_iterator<char>());
	std::cout << Count<ExactFilter>(input) << std::endl;
	std::cout << Count<AnagramFilter>(input) << std::endl;
}
