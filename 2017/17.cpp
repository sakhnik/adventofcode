#include <vector>
#include "../test.hpp"

namespace {

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

unsigned SpinLock2(unsigned step, unsigned iterations)
{
	// It's impractical to track the whole circular list this time.
	// But we can just record what number is next to 0 at every step,
	// while keeping 0 at its original position.

	size_t size{1};   // single 0 initially
	size_t distance_from_zero{0};
	unsigned next_to_zero{0};

	for (unsigned i = 1; i <= iterations; ++i, ++size)
	{
		distance_from_zero = (distance_from_zero + step + 1) % size;
		if (distance_from_zero == 0)
		{
			// We'd need to insert before zero. To simplify the tracking,
			// change reference point to keep zero at the initial position.
			distance_from_zero = size;
		}
		if (distance_from_zero == 1)
			next_to_zero = i;
	}

	return next_to_zero;
}

using namespace boost::ut;

suite s = [] {
	"2017-17"_test = [] {
		expect(SpinLock(3) == 638_u);
		expect(SpinLock2(3, 0) == 0_u);
		expect(SpinLock2(3, 1) == 1_u);
		expect(SpinLock2(3, 2) == 2_u);
		expect(SpinLock2(3, 3) == 2_u);
		expect(SpinLock2(3, 4) == 2_u);
		expect(SpinLock2(3, 5) == 5_u);
		expect(SpinLock2(3, 8) == 5_u);
		expect(SpinLock2(3, 9) == 9_u);

		Printer::Print(__FILE__, "1", SpinLock(371));
		Printer::Print(__FILE__, "2", SpinLock2(371, 50000000));
	};
};

} //namespace;