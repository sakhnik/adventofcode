#include <doctest/doctest.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

namespace {

// Return 0 if not real
int CheckIsReal(const std::string &room)
{
	bool checksum_started{false};
	string checksum;
	int sector{0};
	vector<pair<char, int>> letters;

	for (char ch : room)
	{
		switch (ch)
		{
		case 'a'...'z':
			if (checksum_started)
				checksum.push_back(ch);
			else
			{
				auto it = find_if(begin(letters), end(letters),
								  [ch](auto l) { return l.first == ch; });
				if (it == end(letters))
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

	sort(begin(letters), end(letters),
		 [](auto a, auto b)
		 { return a.second > b.second || (a.second == b.second && a.first < b.first); });

	if (equal(begin(checksum), end(checksum), begin(letters),
			  [](auto a, auto b) { return a == b.first; } ))
	{
		return sector;
	}

	return 0;
}

int Solve(istream &&is)
{
	int sum{0};

	string room;
	while (getline(is, room))
	{
		sum += CheckIsReal(room);
	}

	return sum;
}

string Decrypt(const string &room)
{
	auto s = room.find_first_of("0123456789");
	auto name = room.substr(0, s - 1);
	int sector{0};
	sscanf(&room[s], "%d", &sector);

	for_each(begin(name), end(name),
			[sector](char &ch)
			{
				if (ch == '-')
					return ch = ' ';
				return ch = 'a' + ((ch - 'a' + sector) % ('z' - 'a' + 1));
			});

	return name;
}

int Solve2(istream &&is)
{
	int res{0};

	string room;
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

} //namespace;

TEST_CASE(TEST_NAME)
{
	REQUIRE(123 == CheckIsReal("aaaaa-bbb-z-y-x-123[abxyz]"));
	REQUIRE(987 == CheckIsReal("a-b-c-d-e-f-g-h-987[abcde]"));
	REQUIRE(404 == CheckIsReal("not-a-real-room-404[oarel]"));
	REQUIRE(0 == CheckIsReal("totally-real-room-200[decoy]"));

	cout << Solve(ifstream(INPUT)) << endl;

	REQUIRE("very encrypted name" == Decrypt("qzmt-zixmtkozy-ivhz-343"));

	cout << Solve2(ifstream(INPUT)) << endl;
}
