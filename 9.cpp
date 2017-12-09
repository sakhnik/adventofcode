#include <iostream>
#include <cassert>
#include <string>

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

int main()
{
	assert(Count("{}") == Result(1, 0));
	assert(Count("{{{}}}") == Result(6, 0));
	assert(Count("{{},{}}") == Result(5, 0));
	assert(Count("{{{},{},{{}}}}") == Result(16, 0));
	assert(Count("{<a>,<a>,<a>,<a>}") == Result(1, 4));
	assert(Count("{{<ab>},{<ab>},{<ab>},{<ab>}}") == Result(9, 8));
	assert(Count("{{<!!>},{<!!>},{<!!>},{<!!>}}") == Result(9, 0));
	assert(Count("{{<a!>},{<a!>},{<a!>},{<ab>}}").score == 3);

	std::string str((std::istreambuf_iterator<char>(std::cin)),
					std::istreambuf_iterator<char>());
	std::cout << Count(str.c_str()).score << std::endl;
	std::cout << Count(str.c_str()).garbage << std::endl;
}
