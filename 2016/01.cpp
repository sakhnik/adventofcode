#include <fstream>
#include <string>
#include <sstream>
#include <cassert>
#include <set>
#include "../test.hpp"
namespace {

int walk(std::istream &&is)
{
	enum Direction
	{
		North = 0,
		East,
		South,
		West,

		DirectionCount
	};

	std::pair<int, int> coords{0, 0};
	std::set<decltype(coords)> visited;
	visited.insert(coords);

	auto direction = North;

	auto turn_right = [](Direction d)
	{
		return Direction((d + 1) % DirectionCount);
	};

	auto turn_left = [](Direction d)
	{
		return Direction((d + DirectionCount - 1) % DirectionCount);
	};

	auto move = [](auto coords, Direction d, int steps)
	{
		switch (d)
		{
		case North:
			return std::make_pair(coords.first, coords.second + steps);
		case East:
			return std::make_pair(coords.first + steps, coords.second);
		case South:
			return std::make_pair(coords.first, coords.second - steps);
		default:
			assert(!"Not implemented");
			// fallthrough
		case West:
			return std::make_pair(coords.first - steps, coords.second);
		}
	};


	std::string s;
	while (is >> s)
	{
		char turn = s[0];
		unsigned steps{0};
		sscanf(&s[1], "%d", &steps);

		switch (turn)
		{
		case 'R':
			direction = turn_right(direction);
			break;
		case 'L':
			direction = turn_left(direction);
			break;
		}

		for (; steps; --steps)
		{
			coords = move(coords, direction, 1);
			if (visited.find(coords) != visited.end())
			{
				return abs(coords.first) + abs(coords.second);
			}
			visited.insert(coords);
		}
	}

	return -1;
}

using namespace boost::ut;

suite s = [] {
	"2016-01"_test = [] {
		expect(4_i == walk(std::istringstream{"R8, R4, R4, R8"}));

		Printer::Print(__FILE__, nullptr, walk(std::ifstream{INPUT}));
	};
};

} //namespace;