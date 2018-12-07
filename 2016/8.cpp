#include <doctest/doctest.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include <numeric>

using namespace std;

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

	Screen& Execute(const string &command)
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

	string Dump() const
	{
		ostringstream oss;
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
	vector<int> _pixels;

	int& Pixel(int x, int y)
	{
		return _pixels[y*width + x];
	}

	const int& Pixel(int x, int y) const
	{
		return _pixels[y*width + x];
	}
};

TEST_CASE(TEST_NAME)
{
	Screen test{7, 3};
	REQUIRE(test.Dump() == ".......\n.......\n.......\n");
	test.Execute("rect 3x2");
	REQUIRE(test.Dump() == "###....\n###....\n.......\n");
	test.Execute("rotate column x=1 by 1");
	REQUIRE(test.Dump() == "#.#....\n###....\n.#.....\n");
	test.Execute("rotate row y=0 by 4");
	REQUIRE(test.Dump() == "....#.#\n###....\n.#.....\n");
	test.Execute("rotate column x=1 by 1");
	REQUIRE(test.Dump() == ".#..#.#\n#.#....\n.#.....\n");

	ifstream ifs(INPUT);
	Screen screen{50, 6};
	string line;
	while (getline(ifs, line))
		screen.Execute(line);

	cout << screen.Count() << endl;
	cout << screen.Dump() << endl;
}
