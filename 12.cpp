#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <queue>
#include <fstream>

typedef std::vector<unsigned> AdjT;
typedef std::vector<AdjT> GraphT;

GraphT ReadGraph(std::istream &is)
{
	GraphT graph;

	std::string line;
	while (std::getline(is, line))
	{
		std::istringstream iss(line);

		unsigned v{0};
		iss >> v;
		if (v > graph.size())
		{
			graph.resize(v);
		}
		graph.emplace_back();
		auto &adj = graph[v];

		std::string skip;
		iss >> skip;  // <->

		unsigned w;
		while (iss >> w)
		{
			adj.push_back(w);
			char ch{0};
			iss >> ch;  // ,
		}
	}

	return graph;
}

GraphT ReadGraph(std::istream &&is)
{
	return ReadGraph(is);
}

unsigned Count(const GraphT &graph)
{
	unsigned count{0};
	std::vector<bool> visited(graph.size());
	std::queue<unsigned> q;
	q.push(0);
	visited[0] = true;

	while (!q.empty())
	{
		unsigned v = q.front();
		q.pop();
		visited[v] = true;
		++count;

		for (unsigned w : graph[v])
		{
			if (!visited[w])
			{
				q.push(w);
				visited[w] = true;
			}
		}
	}

	return count;
}

TEST_CASE("main")
{
	auto test_graph = ReadGraph(std::istringstream(R"(0 <-> 2
1 <-> 1
2 <-> 0, 3, 4
3 <-> 2, 4
4 <-> 2, 3, 6
5 <-> 6
6 <-> 4, 5)"));
	REQUIRE(Count(test_graph) == 6);

	std::ifstream ifs(INPUT);
	auto graph = ReadGraph(ifs);
	std::cout << Count(graph) << std::endl;
}
