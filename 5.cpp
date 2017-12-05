#include <iostream>
#include <vector>
#include <cassert>

unsigned CountJumps(std::vector<int> offsets)
{
	unsigned count{0};
	unsigned ip{0};

	try
	{
		while (true)
		{
			auto &offset = offsets.at(ip);
			ip += offset++;
			++count;
		}
	}
	catch (const std::out_of_range &)
	{
		return count;
	}
}

int main()
{
	assert(CountJumps({0, 3, 0, 1, -3}) == 5);

	std::vector<int> offsets;
	int o{0};
	while (std::cin >> o)
		offsets.emplace_back(o);

	std::cout << CountJumps(offsets) << std::endl;
}
