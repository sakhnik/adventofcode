#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "../test.hpp"
namespace {

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

using namespace boost::ut;

suite s = [] {
	"2017-04"_test = [] {
		expect(1_u == Count<ExactFilter>("aa bb cc dd ee"));
		expect(0_u == Count<ExactFilter>("aa bb cc dd aa"));
		expect(1_u == Count<ExactFilter>("aa bb cc dd aaa"));

		std::ifstream ifs(INPUT);
		std::string input((std::istreambuf_iterator<char>(ifs)),
						  std::istreambuf_iterator<char>());
		Printer::Print(__FILE__, "1", Count<ExactFilter>(input));
		Printer::Print(__FILE__, "2", Count<AnagramFilter>(input));
	};
};

} //namespace;