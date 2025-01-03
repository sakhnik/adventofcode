#include <map>
#include "../test.hpp"
namespace {

// Walk the storage in increasing order while checking the predicate on each step.
template <typename PredT>
unsigned Find(PredT pred)
{
	int x{0}, y{0}, a{0};
	unsigned c{1};

	while (true)
	{
		a += 1;
		if (unsigned r = pred(++x, y, ++c))
			return r;
		while (y < a)
		{
			if (unsigned r = pred(x, ++y, ++c))
				return r;
		}
		while (x > -a)
		{
			if (unsigned r = pred(--x, y, ++c))
				return r;
		}
		while (y > -a)
		{
			if (unsigned r = pred(x, --y, ++c))
				return r;
		}
		while (x < a)
		{
			if (unsigned r = pred(++x, y, ++c))
				return r;
		}
	}
}

struct NumberIs
{
	unsigned n;

	NumberIs(unsigned n): n(n) {}

	unsigned operator()(int x, int y, unsigned c)
	{
		if (c == n)
			return (x > 0 ? x : -x) + (y > 0 ? y : -y);
		return 0;
	}
};

struct CheckSumGreatherThan
{
	std::map<std::pair<int,int>, unsigned> storage;
	unsigned target;

	CheckSumGreatherThan(unsigned target)
		: target(target)
	{
		storage[{0,0}] = 1;
	}

	unsigned operator()(int x, int y, unsigned /*c*/)
	{
		auto cs = _CalcCs(x, y);
		if (cs > target)
			return cs;
		storage[{x,y}] = cs;
		return 0;
	}

private:
	unsigned _CalcCs(int x, int y)
	{
		unsigned cs = 0;
		for (int dx = -1; dx <= 1; ++dx)
		{
			for (int dy = -1; dy <= 1; ++dy)
			{
				if (dx || dy)
				{
					auto i = storage.find({x+dx, y+dy});
					if (i != storage.end())
						cs += i->second;
				}
			}
		}
		return cs;
	}
};

using namespace boost::ut;

suite s = [] {
	"03"_test = [] {
		expect(1_u == Find(NumberIs{2}));
		expect(2_u == Find(NumberIs{3}));
		expect(1_u == Find(NumberIs{4}));
		expect(2_u == Find(NumberIs{5}));
		expect(3_u == Find(NumberIs{12}));
		expect(2_u == Find(NumberIs{23}));
		expect(31_u == Find(NumberIs{1024}));

		Printer::Print(__FILE__, "1", Find(NumberIs{289326}));
		Printer::Print(__FILE__, "2", Find(CheckSumGreatherThan{289326}));
	};
};

} //namespace;
