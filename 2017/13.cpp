#include <doctest/doctest.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

typedef std::vector<unsigned> ConfigT;

ConfigT ReadConfig(std::istream &is)
{
	ConfigT config;

	unsigned depth{0}, range{0};
	char skip;
	while (is >> depth >> skip >> range)
	{
		if (config.size() < depth)
			config.resize(depth);
		config.push_back(range);
	}

	return config;
}

ConfigT ReadConfig(std::istream &&is)
{
	return ReadConfig(is);
}

unsigned CalcSeverity(unsigned delay, const ConfigT &config)
{
	unsigned severity{0};

	for (size_t i = 0; i < config.size(); ++i)
	{
		if (!config[i])
			continue;
		unsigned period = 2 * (config[i] - 1);
		if ((i + delay) % period == 0)
		{
			severity += config[i] * i;
		}
	}

	return severity;
}

bool IsCaught(unsigned delay, const ConfigT &config)
{
	for (size_t i = 0; i < config.size(); ++i)
	{
		if (!config[i])
			continue;
		unsigned period = 2 * (config[i] - 1);
		if ((i + delay) % period == 0)
		{
			return true;
		}
	}
	return false;
}

TEST_CASE(TEST_NAME)
{
	auto test_config = ReadConfig(std::istringstream{"0: 3\n1: 2\n4: 4\n6: 4"});
	REQUIRE(CalcSeverity(0, test_config) == 24);

	std::ifstream ifs(INPUT);
	auto config = ReadConfig(ifs);
	MESSAGE(CalcSeverity(0, config));

	for (int delay = 0; ; ++delay)
	{
		if (!IsCaught(delay, config))
		{
			MESSAGE(delay);
			break;
		}
	}
}
