#include <fstream>
#include "../test.hpp"

namespace {

std::pair<unsigned, unsigned> Count(const char *path)
{
	// A good guide to hexagonal grids is here:
	// https://www.redblobgames.com/grids/hexagons/
	// We'll use cube coordinates (a diagonal slice of cube is hex grid too)

	int x{0}, y{0}, z{0};
	auto calc_dist = [&] { return ((x < 0 ? -x : x) + (y < 0 ? -y : y) + (z < 0 ? -z : z)) / 2; };
	unsigned max_dist{0};

	while (char ch = *path++)
	{
		unsigned dist = calc_dist();
		if (dist > max_dist)
			max_dist = dist;

		switch (ch)
		{
		case 'n':
			switch (*path++)
			{
			case ',':
				// n
				++y;
				--z;
				break;
			case 'e':
				// ne
				++x;
				--z;
				break;
			case 'w':
				// nw
				++y;
				--x;
				break;
			}
			break;
		case 's':
			switch (*path++)
			{
			case ',':
				// s
				--y;
				++z;
				break;
			case 'e':
				// se
				++x;
				--y;
				break;
			case 'w':
				// sw
				--x;
				++z;
				break;
			}
			break;
		}
	}

	return { calc_dist(), max_dist };
}

using namespace boost::ut;

suite s = [] {
	"2017-11"_test = [] {
		expect(3_u == Count("ne,ne,ne,").first);
		expect(0_u == Count("ne,ne,sw,sw,").first);
		expect(2_u == Count("ne,ne,s,s,").first);
		expect(3_u == Count("se,sw,se,sw,sw,").first);

		std::ifstream ifs(INPUT);
		std::string input;
		std::getline(ifs, input);
		input.push_back(',');

		auto res = Count(input.c_str());
		Printer::Print(__FILE__, "1", res.first);
		Printer::Print(__FILE__, "2", res.second);
	};
};

} //namespace;