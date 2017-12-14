#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>

#include "KnotHash.h"

unsigned Count(unsigned size, const std::vector<unsigned> &lengths)
{
	Circle circle(size);

	unsigned position{0};
	unsigned skip{0};

	for (unsigned l : lengths)
	{
		circle.Reverse(position, position + l);
		position += l + skip++;
	}

	return circle._buf[0] * circle._buf[1];
}

std::string Hash(const std::string &input)
{
	const char xdigits[] = "0123456789abcdef";
	std::string hash;
	for (uint8_t x : KnotHash(input))
	{
		hash.push_back(xdigits[x >> 4]);
		hash.push_back(xdigits[x & 0xf]);
	}
	return hash;
}

TEST_CASE("main")
{
	REQUIRE(Count(5, {3, 4, 1, 5}) == 12);
	REQUIRE(Hash("") == "a2582a3a0e66e6e86e3812dcb672a272");
	REQUIRE(Hash("AoC 2017") == "33efeb34ea91902bb2f59c9920caa6cd");
	REQUIRE(Hash("1,2,3") == "3efbe78a8d82f29979031a4aa0b16a9d");
	REQUIRE(Hash("1,2,4") == "63960835bcdc130f0b66d7ff4f6a5a8e");

	unsigned lengths[] = {
		225,171,131,2,35,5,0,13,1,246,54,97,255,98,254,110
	};

	std::cout << Count(256, {lengths, lengths + std::size(lengths)}) << std::endl;

	std::cout << Hash("225,171,131,2,35,5,0,13,1,246,54,97,255,98,254,110") << std::endl;
}
