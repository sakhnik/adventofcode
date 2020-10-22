#include <fstream>
#include <vector>
#include "../test.hpp"
namespace {

template <typename ModifierT>
unsigned CountJumps(std::vector<int> offsets, ModifierT modifier)
{
	unsigned count{0};
	unsigned ip{0};

	try
	{
		while (true)
		{
			auto &offset = offsets.at(ip);
			ip += modifier(offset);
			++count;
		}
	}
	catch (const std::out_of_range &)
	{
		return count;
	}
}

int IncreaseModifier(int &offset)
{
	return offset++;
}

int WeirdModifier(int &offset)
{
	return offset >= 3 ? offset-- : offset++;
}

using namespace boost::ut;

suite s = [] {
	"2017-05"_test = [] {
		expect(5_u == CountJumps({0, 3, 0, 1, -3}, IncreaseModifier));

		std::ifstream ifs(INPUT);
		std::vector<int> offsets;
		int o{0};
		while (ifs >> o)
			offsets.emplace_back(o);

		Printer::Print(__FILE__, "1", CountJumps(offsets, IncreaseModifier));
		Printer::Print(__FILE__, "2", CountJumps(offsets, WeirdModifier));
	};
};

} //namespace;