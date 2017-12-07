#include <iostream>
#include <unordered_map>
#include <sstream>
#include <cassert>

struct Data
{
	unsigned weight;
	std::string parent;
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

int main()
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

	assert(FindRoot(ParseInput(std::istringstream(test))) == "tknk");

	auto tree = ParseInput(std::cin);
	std::cout << FindRoot(tree) << std::endl;
}
