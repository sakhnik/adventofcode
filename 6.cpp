#define CATCH_CONFIG_MAIN
#include <catch.hpp>
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
std::pair<unsigned, unsigned>
Count(T banks)
{
	unsigned count{0};
	std::unordered_map<T, unsigned, MyHash<T>> seen;
	seen.insert({banks, count});
	while (true)
	{
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
		}

		{
			auto [it, did] = seen.insert({banks, count});
			if (!did)
				return {count, count - it->second};
		}
	}
}

TEST_CASE("main", "")
{
	REQUIRE(Count(std::array<unsigned,4>({0, 2, 7, 0})).first == 5);
	REQUIRE(Count(std::array<unsigned,4>({0, 2, 7, 0})).second == 4);

	std::array<unsigned, 16> banks;
	for (auto &i : banks)
		std::cin >> i;

	auto res = Count(banks);
	std::cout << res.first << std::endl;
	std::cout << res.second << std::endl;
}
