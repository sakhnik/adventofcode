#include <doctest/doctest.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <limits>
#include <queue>
#include <algorithm>
#include <numeric>
#include <cassert>


namespace {

struct Poi
{
	int num;
	int x;
	int y;
};

typedef std::vector<Poi> PoiT;
typedef std::vector<std::string> MazeT;

// Search for shortest distances from the given poi to other
std::vector<int> Bfs(int num, const MazeT &maze, const PoiT &poi)
{
	// Map of visited flags
	std::vector<bool> visited(maze.size() * maze.front().size(), false);
	auto idx = [&](int x, int y) { return x * maze.size() + y; };

	// Vector of distances between POI
	std::vector<int> dist(poi.size(), std::numeric_limits<int>::max());

	// Search queue for BFS
	struct Cur
	{
		int x,y,dist;
	};

	std::queue<Cur> to_search;
	to_search.push({poi[num].x, poi[num].y, 0});
	dist[num] = 0;
	visited[idx(poi[num].x, poi[num].y)] = true;

	size_t visited_poi = 1;
	while (!to_search.empty() && visited_poi != poi.size())
	{
		auto cur = to_search.front();
		to_search.pop();

		auto try_go = [&](int dx, int dy)
		{
			int new_x = cur.x + dx;
			int new_y = cur.y + dy;
			if (new_x < 0 || new_x >= (int)maze[0].size())
				return;
			if (new_y < 0 || new_y >= (int)maze.size())
				return;

			if (visited[idx(new_x, new_y)])
				return;
			visited[idx(new_x, new_y)] = true;

			char ch = maze[new_y][new_x];
			if (ch == '#')
				return;

			if (isdigit(ch))
			{
				++visited_poi;
				int num = ch - '0';
				assert(poi[num].num == num);
				assert(poi[num].x == new_x);
				assert(poi[num].y == new_y);
				dist[num] = cur.dist + 1;
			}

			to_search.push({new_x, new_y, cur.dist + 1});
		};

		try_go(-1, 0);
		try_go(1, 0);
		try_go(0, -1);
		try_go(0, 1);
	}

	return dist;
}

std::vector<std::vector<int>> GetDistances(std::istream &is)
{
	PoiT poi;
	MazeT maze;

	std::string line;
	while (getline(is, line))
	{
		for (size_t x = 0; x != line.size(); ++x)
			if (isdigit(line[x]))
			{
				Poi p;
				p.num = line[x] - '0';
				p.x = x;
				p.y = maze.size();
				poi.push_back(p);
			}
		maze.push_back(line);
	}

	sort(begin(poi), end(poi), [](auto a, auto b) { return a.num < b.num; });

	// Find distances between all the POI
	std::vector<std::vector<int>> distances;
	for (int i{}, n = poi.size(); i != n; ++i)
		distances.emplace_back(Bfs(i, maze, poi));

	return distances;
}

int Solve(std::istream &&is)
{
	auto distances = GetDistances(is);

	// Try all the permutations between POI 1..n
	std::vector<int> idx(distances.size());
	iota(begin(idx), end(idx), 0);

	int min_distance = std::numeric_limits<int>::max();
	do
	{
		int distance{};
		for (int i = 0, n = idx.size() - 1; i != n; ++i)
			distance += distances[idx[i]][idx[i + 1]];
		if (distance < min_distance)
			min_distance = distance;
	}
	while (next_permutation(begin(idx) + 1, end(idx)));

	return min_distance;
}

int Solve2(std::istream &&is)
{
	auto distances = GetDistances(is);

	// Try all the permutations between POI 1..n
	std::vector<int> idx(distances.size());
	iota(begin(idx), end(idx), 0);
	idx.push_back(0);

	int min_distance = std::numeric_limits<int>::max();
	do
	{
		assert(idx.front() == 0 && idx.back() == 0);

		int distance{};
		for (int i = 0, n = idx.size() - 1; i != n; ++i)
			distance += distances[idx[i]][idx[i + 1]];
		if (distance < min_distance)
			min_distance = distance;
	}
	while (next_permutation(begin(idx) + 1, begin(idx) + distances.size()));

	return min_distance;
}

} //namespace;

TEST_CASE(TEST_NAME)
{
	const char *test_maze =
R"(###########
#0.1.....2#
#.#######.#
#4.......3#
###########")";
	REQUIRE(14 == Solve(std::istringstream{test_maze}));

	MESSAGE(Solve(std::ifstream{INPUT}));
	MESSAGE(Solve2(std::ifstream{INPUT}));
}
