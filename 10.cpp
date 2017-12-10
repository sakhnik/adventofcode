#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <vector>
#include <numeric>

class Circle
{
	std::vector<unsigned> _circle;

public:
	Circle(unsigned size)
		: _circle(size)
	{
		std::iota(_circle.begin(), _circle.end(), 0);
	}

	void Reverse(unsigned i, unsigned j)
	{
		for (--j; i < j; ++i, --j)
		{
			std::swap(_circle[i % _circle.size()], _circle[j % _circle.size()]);
		}
	}

	unsigned GetAt(unsigned i) const
	{
		return _circle[i % _circle.size()];
	}
};

unsigned Count(unsigned size, const std::vector<unsigned> &lengths)
{
	Circle circle(size);

	unsigned position{0};
	unsigned skip{0};

	for (unsigned l : lengths)
	{
		circle.Reverse(position, position + l);
		position += l + skip++;
	}

	return circle.GetAt(0) * circle.GetAt(1);
}

TEST_CASE("main")
{
	REQUIRE(Count(5, {3, 4, 1, 5}) == 12);

	unsigned lengths[] = {
		225,171,131,2,35,5,0,13,1,246,54,97,255,98,254,110
	};

	std::cout << Count(256, {lengths, lengths + std::size(lengths)}) << std::endl;
}
