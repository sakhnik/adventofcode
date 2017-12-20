#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <fstream>
#include <algorithm>

struct Vec
{
	int x, y, z;
};

Vec& operator+=(Vec &v1, const Vec &v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}

struct Part
{
	Vec p, v, a;
	unsigned number;
};

typedef std::vector<Part> SwarmT;

SwarmT Parse(std::istream &is)
{
	SwarmT swarm;
	std::string line;
	unsigned number{0};
	while (getline(is, line))
	{
		Part part;
		part.number = number++;
		sscanf(line.c_str(),
			   "p=<%d,%d,%d>, v=<%d,%d,%d>, a=<%d,%d,%d>",
			   &part.p.x, &part.p.y, &part.p.z,
			   &part.v.x, &part.v.y, &part.v.z,
			   &part.a.x, &part.a.y, &part.a.z);
		swarm.push_back(part);
	}
	return swarm;
}

unsigned Distance(const Vec &a)
{
	return std::abs(a.x) + std::abs(a.y) + std::abs(a.z);
}

SwarmT FilterLowestAcceleration(SwarmT swarm)
{
	std::sort(swarm.begin(), swarm.end(),
			  [](const auto &p1, const auto &p2) { return Distance(p1.a) < Distance(p2.a); });
	auto it = std::remove_if(swarm.begin(), swarm.end(),
							 [&](const auto &p) { return Distance(p.a) > Distance(swarm[0].a); });
	swarm.erase(it, swarm.end());
	return swarm;
}

unsigned Simulate(SwarmT swarm)
{
	for (unsigned i = 0; i < 1000000; ++i)
	{
		for (auto &p : swarm)
		{
			p.v += p.a;
			p.p += p.v;
		}

	}

	auto it = std::min_element(swarm.begin(), swarm.end(),
							   [](const auto &p1, const auto &p2) { return Distance(p1.p) < Distance(p2.p); });
	return it->number;
}

TEST_CASE("1")
{
}

TEST_CASE("main")
{
	std::ifstream ifs(INPUT);
	auto swarm = Parse(ifs);
	swarm = FilterLowestAcceleration(swarm);
	std::cout << Simulate(swarm) << std::endl;
}
