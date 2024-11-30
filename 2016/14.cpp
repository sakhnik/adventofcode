#include <openssl/md5.h>
#include <vector>
#include <cassert>
#include <algorithm>
#include <boost/circular_buffer.hpp>
#include "../test.hpp"

namespace {

void Md5ToString(const uint8_t *hash, std::string &ret)
{
	if (ret.size() != MD5_DIGEST_LENGTH * 2)
		ret.resize(MD5_DIGEST_LENGTH * 2);

	char *out = &ret[0];
	for (int i = 0; i != MD5_DIGEST_LENGTH; ++i, ++hash)
	{
		static const char digits[] = "0123456789abcdef";
		*out++ = digits[(*hash >> 4) & 0xf];
		*out++ = digits[*hash & 0xf];
	}
}

struct Feature
{
	std::string hash;
	char triplet = 0;
	std::vector<char> fives;
};

Feature CalcFeatures(const std::string &md5)
{
	Feature f;
	f.hash = md5;

	char cur_char = 0;
	int run_length{0};

	auto check_run_length = [&f](char cur_char, int run_length)
	{
		if (run_length >= 3 && !f.triplet)
			f.triplet = cur_char;
		if (run_length >= 5)
			f.fives.push_back(cur_char);
	};

	for (auto ch : md5)
	{
		if (cur_char != ch)
		{
			check_run_length(cur_char, run_length);
			cur_char = ch;
			run_length = 1;
		}
		else
			++run_length;
	}

	check_run_length(cur_char, run_length);
	return f;
}

int Solve(const char *salt, int hash_count)
{
	const int LOOK_AHEAD = 1001;
	const int KEY_COUNT = 64;

	int counter = 0;
	boost::circular_buffer<Feature> hashes(LOOK_AHEAD);

	auto calc_next_hash = [&]()
	{
		uint8_t md5[MD5_DIGEST_LENGTH];
		std::string s = salt + std::to_string(counter++);
		for (int i = 0; i != hash_count; ++i)
		{
			MD5(reinterpret_cast<const uint8_t *>(s.data()), s.size(), md5);
			Md5ToString(md5, s);
		}
		hashes.push_back(CalcFeatures(s));
	};

	while (hashes.size() != LOOK_AHEAD)
		calc_next_hash();

	int key = 0;

	while (true)
	{
		assert(hashes.size() == LOOK_AHEAD);

		const auto &f = hashes.front();
		if (!f.triplet)
		{
			calc_next_hash();
			continue;
		}

		auto it = find_if(begin(hashes) + 1, end(hashes),
			[=](const auto &a)
			{
				return find(begin(a.fives), end(a.fives), f.triplet) != a.fives.end();
			});
		if (it != end(hashes))
		{
			if (++key == KEY_COUNT)
				return counter - LOOK_AHEAD;
		}

		calc_next_hash();
	}
}

using namespace boost::ut;

suite s = [] {
	"14"_test = [] {
		if (Printer::EnableAll())
		{
			expect(22728_i == Solve("abc", 1));
			Printer::Print(__FILE__, "1", Solve("cuanljph", 1));

			expect(22551_i == Solve("abc", 2017));
			Printer::Print(__FILE__, "2", Solve("cuanljph", 2017));
		}
	};
};

} //namespace;
