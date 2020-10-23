#include "../test.hpp"

namespace {

unsigned Count(uint64_t a, uint64_t b)
{
	unsigned judge{0};

	for (unsigned i = 0; i < 40000000; ++i)
	{
		a = (a * 16807) % 2147483647;
		b = (b * 48271) % 2147483647;
		if ((a & 0xffff) == (b & 0xffff))
			++judge;
	}

	return judge;
}

unsigned Count2(uint64_t a, uint64_t b)
{
	unsigned judge{0};

	for (unsigned i = 0; i < 5000000; ++i)
	{
		do
		{
			a = (a * 16807) % 2147483647;
		}
		while (a & 3);

		do
		{
			b = (b * 48271) % 2147483647;
		}
		while (b & 7);

		if ((a & 0xffff) == (b & 0xffff))
			++judge;
	}

	return judge;
}

using namespace boost::ut;

suite s = [] {
	"2017-15"_test = [] {
		expect(588_u == Count(65, 8921));
		expect(309_u == Count2(65, 8921));

		Printer::Print(__FILE__, "1", Count(289, 629));
		Printer::Print(__FILE__, "2", Count2(289, 629));
	};
};

} //namespace;