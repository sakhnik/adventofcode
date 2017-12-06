#include <iostream>
#include <cassert>
#include <array>
#include <algorithm>
#include <unordered_set>
#include <boost/functional/hash.hpp>

template <typename T>
struct MyHash
{
	typedef T argument_type;
	typedef std::size_t result_type;
	result_type operator()(argument_type const& s) const noexcept
	{
		result_type seed{0};
		for (auto i : s)
			boost::hash_combine(seed, i);
		return seed;
	}
};

template <typename T>
unsigned Count(T banks)
{
	unsigned count{0};
	std::unordered_set<T, MyHash<T>> seen;
	while (true)
	{
		auto it = std::max_element(banks.begin(), banks.end());
		unsigned spare = *it;
		*it = 0;
		//unsigned portion = spare / banks.size();
		//unsigned rest = spare % banks.size();
		while (spare--)
		{
			if (++it == banks.end())
				it = banks.begin();
			++*it;
		}
		++count;
		if (!seen.insert(banks).second)
			return count;
	}
}

int main()
{
	assert(Count(std::array<unsigned,4>({0, 2, 7, 0})) == 5);

	std::array<unsigned, 16> banks;
	for (auto &i : banks)
		std::cin >> i;

	std::cout << Count(banks) << std::endl;
}
