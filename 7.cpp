#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>

struct Data
{
	unsigned weight = 0;
	std::string parent;
	std::vector<std::string> children;
};

typedef std::unordered_map<std::string, Data> TreeT;

TreeT ParseInput(std::istream &is)
{
	TreeT tree;
	std::string line;
	while (std::getline(is, line))
	{
		std::istringstream s(line);
		std::string name;
		s >> name;
		char c;
		s >> c >> tree[name].weight >> c;

		std::string child;
		s >> child;   // arrow
		while (s >> child)
		{
			if (child.back() == ',')
				child.pop_back();
			tree[child].parent = name;
			tree[name].children.push_back(child);
		}
	}

	return tree;
}

TreeT ParseInput(std::istream &&is)
{
	return ParseInput(is);
}

std::string FindRoot(const TreeT &tree)
{
	auto it = tree.begin();
	while (!it->second.parent.empty())
		it = tree.find(it->second.parent);
	return it->first;
}

// total
unsigned CheckWeight(const TreeT &tree, const std::string &node)
{
	auto it = tree.find(node);
	if (it->second.children.empty())
		return it->second.weight;

	std::vector<unsigned> weight;
	for (const auto &child : it->second.children)
		weight.push_back(CheckWeight(tree, child));

	// calculate median
	std::vector<unsigned> weight2(weight);
	auto it_med = weight2.begin() + weight2.size() / 2;
	std::nth_element(weight2.begin(), it_med, weight2.end());
	auto median = *it_med;

	unsigned total = std::accumulate(weight.begin(), weight.end(), 0);
	auto j = std::find_if(weight.begin(), weight.end(), [=](unsigned w) { return w != median; });
	if (j != weight.end())
	{
		unsigned idx = j - weight.begin();
		auto it_ch = tree.find(it->second.children[idx]);
		throw median - weight[idx] + it_ch->second.weight;
	}

	std::sort(weight.begin(), weight.end());
	unsigned expect = weight[weight.size()/2];
	if (weight.front() != expect || weight.back() != expect)
		throw expect;

	return it->second.weight + total;
}

TEST_CASE("main")
{
	const char *const test =
		R"(pbga (66)
xhth (57)
ebii (61)
havc (66)
ktlj (57)
fwft (72) -> ktlj, cntj, xhth
qoyq (66)
padx (45) -> pbga, havc, qoyq
tknk (41) -> ugml, padx, fwft
jptl (61)
ugml (68) -> gyxo, ebii, jptl
gyxo (61)
cntj (57))";

	auto testTree = ParseInput(std::istringstream(test));
	REQUIRE(FindRoot(testTree) == "tknk");
	try
	{
		CheckWeight(testTree, FindRoot(testTree));
		REQUIRE(!"Should get here");
	}
	catch (unsigned res)
	{
		REQUIRE(res == 60);
	}

	std::ifstream ifs(INPUT);
	auto tree = ParseInput(ifs);
	auto root = FindRoot(tree);
	std::cout << root << std::endl;
	try
	{
		CheckWeight(tree, root);
	}
	catch (unsigned res)
	{
		std::cout << res << std::endl;
	}
}
