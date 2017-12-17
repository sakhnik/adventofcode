#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>

unsigned SpinLock(unsigned step)
{
	std::vector<unsigned> ring{0};
	size_t cur{0};
	for (unsigned i = 1; i <= 2017; ++i)
	{
		cur = (cur + step + 1) % ring.size();
		ring.insert(ring.begin() + cur, i);
	}

	cur = (cur + 1) % 2017;
	return ring[cur];
}

TEST_CASE("main")
{
	REQUIRE(SpinLock(3) == 638);
	std::cout << SpinLock(371) << std::endl;
}
