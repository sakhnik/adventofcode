#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <bitset>
#include "KnotHash.h"

unsigned Count(const std::string &key)
{
	unsigned count{0};
	for (unsigned i = 0; i < 128; ++i)
	{
		auto h = KnotHash(key + '-' + std::to_string(i));
		for (char x : h)
			count += std::bitset<8>(x).count();
	}
	return count;
}

TEST_CASE("main")
{
	std::cout << Count("ffayrhll") << std::endl;
}
