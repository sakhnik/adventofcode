#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <fstream>

struct Result
{
	Result(unsigned score = 0, unsigned garbage = 0)
		: score(score)
		, garbage(garbage)
	{
	}

	bool operator==(const Result &o) const
	{
		return score == o.score && garbage == o.garbage;
	}

	unsigned score;
	unsigned garbage;
};

Result Count(const char *s)
{
	Result result;
	unsigned depth{0};

	while (true)
	{
		switch (char ch = *s++)
		{
		case 0:
			return result;
		case '{':
			result.score += ++depth;
			break;
		case '}':
			--depth;
			break;
		case '<':
			for (char g = *s++; g != '>'; g = *s++)
			{
				if (g == '!')
					++s;
				else
					++result.garbage;
			}
			break;
		}
	}
}

TEST_CASE("main")
{
	REQUIRE(Count("{}") == Result(1, 0));
	REQUIRE(Count("{{{}}}") == Result(6, 0));
	REQUIRE(Count("{{},{}}") == Result(5, 0));
	REQUIRE(Count("{{{},{},{{}}}}") == Result(16, 0));
	REQUIRE(Count("{<a>,<a>,<a>,<a>}") == Result(1, 4));
	REQUIRE(Count("{{<ab>},{<ab>},{<ab>},{<ab>}}") == Result(9, 8));
	REQUIRE(Count("{{<!!>},{<!!>},{<!!>},{<!!>}}") == Result(9, 0));
	REQUIRE(Count("{{<a!>},{<a!>},{<a!>},{<ab>}}").score == 3);

	std::ifstream ifs(INPUT);
	std::string str((std::istreambuf_iterator<char>(ifs)),
					std::istreambuf_iterator<char>());
	std::cout << Count(str.c_str()).score << std::endl;
	std::cout << Count(str.c_str()).garbage << std::endl;
}
