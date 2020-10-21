#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <boost/ut.hpp>

#include "../Printer.hpp"

namespace {

// Return 0 if not real
int CheckIsReal(const std::string &room)
{
	bool checksum_started{false};
	std::string checksum;
	int sector{0};
	std::vector<std::pair<char, int>> letters;

	for (char ch : room)
	{
		switch (ch)
		{
		case 'a'...'z':
			if (checksum_started)
				checksum.push_back(ch);
			else
			{
				auto it = std::find_if(begin(letters), end(letters),
								  [ch](auto l) { return l.first == ch; });
				if (it == letters.end())
					letters.emplace_back(ch, 1);
				else
					++it->second;
			}
			break;
		case '[':
			checksum_started = true;
			break;
		case '0'...'9':
			sector *= 10;
			sector += ch - '0';
			break;
		}
	}

	assert(checksum.size() == 5);

	std::sort(begin(letters), end(letters),
		 [](auto a, auto b)
		 { return a.second > b.second || (a.second == b.second && a.first < b.first); });

	if (std::equal(begin(checksum), end(checksum), begin(letters),
			  [](auto a, auto b) { return a == b.first; } ))
	{
		return sector;
	}

	return 0;
}

int Solve(std::istream &&is)
{
	int sum{0};

	std::string room;
	while (getline(is, room))
	{
		sum += CheckIsReal(room);
	}

	return sum;
}

std::string Decrypt(const std::string &room)
{
	auto s = room.find_first_of("0123456789");
	auto name = room.substr(0, s - 1);
	int sector{0};
	sscanf(&room[s], "%d", &sector);

	std::for_each(begin(name), end(name),
			[sector](char &ch)
			{
				if (ch == '-')
					return ch = ' ';
				return ch = 'a' + ((ch - 'a' + sector) % ('z' - 'a' + 1));
			});

	return name;
}

int Solve2(std::istream &&is)
{
	int res{0};

	std::string room;
	while (getline(is, room))
	{
		res = CheckIsReal(room);
		if (!res)
			continue;

		room = Decrypt(room);
		if (room.find("north") != room.npos)
			return res;
	}

	return 0;
}

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
	"2016-04"_test = [] {
		expect(123_i == CheckIsReal("aaaaa-bbb-z-y-x-123[abxyz]"));
		expect(987_i == CheckIsReal("a-b-c-d-e-f-g-h-987[abcde]"));
		expect(404_i == CheckIsReal("not-a-real-room-404[oarel]"));
		expect(0_i == CheckIsReal("totally-real-room-200[decoy]"));

		Printer::Print(__FILE__, "1", Solve(std::ifstream{INPUT}));

		expect(eq("very encrypted name"s, Decrypt("qzmt-zixmtkozy-ivhz-343")));

		Printer::Print(__FILE__, "2", Solve2(std::ifstream{INPUT}));
	};
};

} //namespace;