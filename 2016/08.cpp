#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include <numeric>
#include <boost/ut.hpp>

namespace {

class Screen
{
public:
	Screen(int width, int height)
		: width(width)
		, height(height)
		, _pixels(width * height, 0)
	{
	}

	int Count() const
	{
		return std::accumulate(begin(_pixels), end(_pixels), 0);
	}

	Screen& Execute(const std::string &command)
	{
		int A{0}, B{0};

		if (2 == sscanf(command.c_str(), "rect %dx%d", &A, &B))
		{
			for (int y{0}; y != B; ++y)
				for (int x{0}; x != A; ++x)
					Pixel(x, y) = 1;
		}

		if (2 == sscanf(command.c_str(), "rotate row y=%d by %d", &A, &B))
		{
			B %= width;
			int buffer[width];
			for (int x{0}; x != width; ++x)
				buffer[(x + B) % width] = Pixel(x, A);
			for (int x{0}; x != width; ++x)
				Pixel(x, A) = buffer[x];
		}

		if (2 == sscanf(command.c_str(), "rotate column x=%d by %d", &A, &B))
		{
			B %= height;
			int buffer[height];
			for (int y{0}; y != height; ++y)
				buffer[(y + B) % height] = Pixel(A, y);
			for (int y{0}; y != height; ++y)
				Pixel(A, y) = buffer[y];
		}

		return *this;
	}

	std::string Dump() const
	{
		std::ostringstream oss;
		for (int y{0}; y != height; ++y)
		{
			for (int x{0}; x != width; ++x)
				oss << (Pixel(x, y) ? '#' : '.');
			oss << '\n';
		}
		return oss.str();
	}

private:
	int width;
	int height;
	std::vector<int> _pixels;

	int& Pixel(int x, int y)
	{
		return _pixels[y*width + x];
	}

	const int& Pixel(int x, int y) const
	{
		return _pixels[y*width + x];
	}
};

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
	"2016-08"_test = [] {
		Screen test{7, 3};
		expect(eq(test.Dump(), ".......\n.......\n.......\n"s));
		test.Execute("rect 3x2");
		expect(eq(test.Dump(), "###....\n###....\n.......\n"s));
		test.Execute("rotate column x=1 by 1");
		expect(eq(test.Dump(), "#.#....\n###....\n.#.....\n"s));
		test.Execute("rotate row y=0 by 4");
		expect(eq(test.Dump(), "....#.#\n###....\n.#.....\n"s));
		test.Execute("rotate column x=1 by 1");
		expect(eq(test.Dump(), ".#..#.#\n#.#....\n.#.....\n"s));

		std::ifstream ifs(INPUT);
		Screen screen{50, 6};
		std::string line;
		while (getline(ifs, line))
			screen.Execute(line);

		std::cout << "2016-08.1: " << screen.Count() << std::endl;
		std::cout << "2016-08.2:\n" << screen.Dump() << std::endl;
	};
};

} //namespace;