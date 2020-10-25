#include <fstream>
#include <regex>
#include <vector>
#include <cassert>
#include <queue>
#include "../test.hpp"

namespace {

using namespace boost::ut;

void Solve(std::istream &&is)
{
	struct Data
	{
		int x, y;
		int avail, used;

		int distance = -1;
	};


	std::vector<Data> lines;
	std::string line;
	while (getline(is, line))
	{
		static std::regex r(R"(/dev/grid/node-x(\d+)-y(\d+)\s+\d+T\s+(\d+)T\s+(\d+)T)");
		std::smatch m;
		if (!regex_search(line, m, r))
			continue;
		assert(m.size() == 5);

		Data d;
		d.x = stoi(m[1]);
		d.y = stoi(m[2]);
		d.used = stoi(m[3]);
		d.avail = stoi(m[4]);

		lines.push_back(d);
	}

	int count{};

	for (size_t i = 0; i != lines.size(); ++i)
		for (size_t j = 0; j != lines.size(); ++j)
			if (i != j && lines[i].used && lines[i].used <= lines[j].avail)
				++count;

	Printer::Print(__FILE__, "1", count);

	///////////////////////////////// Part 2 //////////////////////

	int width{}, height{};
	int _x{-1}, _y{-1}/*, capacity{}*/;

	for (auto d : lines)
	{
		if (width < d.x)
			width = d.x;
		if (height < d.y)
			height = d.y;

		if (d.used == 0)
		{
			_x = d.x;
			_y = d.y;
			//capacity = d.avail;
		}
	}

	++width;
	++height;
	int Gx{width - 1}, Gy{0};

	// Alignment should be the same as input lines
	auto idx = [height](int x, int y)
	{
		return x * height + y;
	};

	std::queue<std::pair<int,int>> to_search;
	to_search.emplace(_x, _y);
	lines[idx(_x, _y)].distance = 0;

	while (!to_search.empty())
	{
		auto xy = to_search.front();
		to_search.pop();

		if (xy.first == Gx && xy.second == Gy)
			break;

		auto try_go = [&](int dx, int dy)
		{
			auto x = xy.first + dx;
			auto y = xy.second + dy;
			if (x < 0 || x >= width)
				return;
			if (y < 0 || y >= height)
				return;
			auto &src = lines[idx(xy.first, xy.second)];
			assert(src.x == xy.first && src.y == xy.second);
			auto &dst = lines[idx(x, y)];
			assert(dst.x == x && dst.y == y);

			if (dst.used <= (src.avail+src.used) && dst.distance == -1)
			{
				dst.distance = src.distance + 1;
				to_search.emplace(x, y);
			}
		};


		try_go(-1, 0);
		try_go(1, 0);
		try_go(0, -1);
		try_go(0, 1);
	}

	auto distance = lines[idx(Gx, Gy)].distance;
	assert(distance != -1);
	distance += 5 * (width - 2);
	Printer::Print(__FILE__, "2", distance);
}

suite s = [] {
	"2016-22"_test = [] {
		Solve(std::ifstream{INPUT});
	};
};

} //namespace;