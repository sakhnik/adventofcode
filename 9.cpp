#include <iostream>
#include <cassert>
#include <string>

unsigned Count(const char *s)
{
	unsigned score{0};
	unsigned depth{0};

	while (true)
	{
		switch (char ch = *s++)
		{
		case 0:
			return score;
		case '{':
			score += ++depth;
			break;
		case '}':
			--depth;
			break;
		case '<':
			for (char g = *s++; g != '>'; g = *s++)
			{
				if (g == '!')
					++s;
			}
			break;
		}
	}
}

int main()
{
	assert(Count("{}") == 1);
	assert(Count("{{{}}}") == 6);
	assert(Count("{{},{}}") == 5);
	assert(Count("{{{},{},{{}}}}") == 16);
	assert(Count("{<a>,<a>,<a>,<a>}") == 1);
	assert(Count("{{<ab>},{<ab>},{<ab>},{<ab>}}") == 9);
	assert(Count("{{<!!>},{<!!>},{<!!>},{<!!>}}") == 9);
	assert(Count("{{<a!>},{<a!>},{<a!>},{<ab>}}") == 3);

	std::string str((std::istreambuf_iterator<char>(std::cin)),
					std::istreambuf_iterator<char>());
	std::cout << Count(str.c_str()) << std::endl;
}
