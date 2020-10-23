#include <openssl/md5.h>
#include <cassert>
#include <string>
#include <limits>
#include "../test.hpp"

namespace {

const int UP = 1;
const int DOWN = 2;
const int LEFT = 4;
const int RIGHT = 8;

int GetDoors(const std::string &path)
{
	int doors{0};

	uint8_t md5[MD5_DIGEST_LENGTH];

	MD5(reinterpret_cast<const uint8_t *>(path.data()), path.size(), md5);
	if ((md5[0] >> 4) > 0xa)
		doors |= UP;
	if ((md5[0] & 0xf) > 0xa)
		doors |= DOWN;
	if ((md5[1] >> 4) > 0xa)
		doors |= LEFT;
	if ((md5[1] & 0xf) > 0xa)
		doors |= RIGHT;

	return doors;
}

int Search(int x, int y, int length, std::string &path, std::string &min_path)
{
	if (x == 4 && y == 4)
	{
		if (min_path.empty() || min_path.size() > path.size())
			min_path = path;
		return length;
	}

	// We'll search depth-first preferring down-right direction.
	// Hope to reach the target sooner.
	// Then we'll cut off the other branches basing on already known
	// distance, thus exhausting the search tree.

	// Distance to the target
	int min_length = std::numeric_limits<int>::max();

	int doors = GetDoors(path);

	if ((doors & DOWN) && y < 4 && length < min_length)
	{
		path.push_back('D');
		auto l = Search(x, y + 1, length + 1, path, min_path);
		path.pop_back();
		if (l < min_length)
			min_length = l;
	}

	if ((doors & RIGHT) && x < 4 && length < min_length)
	{
		path.push_back('R');
		auto l = Search(x + 1, y, length + 1, path, min_path);
		path.pop_back();
		if (l < min_length)
			min_length = l;
	}

	if ((doors & UP) && y > 1 && length < min_length)
	{
		path.push_back('U');
		auto l = Search(x, y - 1, length + 1, path, min_path);
		path.pop_back();
		if (l < min_length)
			min_length = l;
	}

	if ((doors & LEFT) && x > 1 && length < min_length)
	{
		path.push_back('L');
		auto l = Search(x - 1, y, length + 1, path, min_path);
		path.pop_back();
		if (l < min_length)
			min_length = l;
	}

	return min_length;
}

int SearchMax(int x, int y, int length, std::string &path)
{
	if (x == 4 && y == 4)
		return length;

	// We'll search depth-first preferring down-right direction.
	// Hope to reach the target sooner.
	// Then we'll cut off the other branches basing on already known
	// distance, thus exhausting the search tree.

	// Distance to the target
	int max_length = 0;

	int doors = GetDoors(path);

	if ((doors & DOWN) && y < 4)
	{
		path.push_back('D');
		auto l = SearchMax(x, y + 1, length + 1, path);
		path.pop_back();
		if (l > max_length)
			max_length = l;
	}

	if ((doors & RIGHT) && x < 4)
	{
		path.push_back('R');
		auto l = SearchMax(x + 1, y, length + 1, path);
		path.pop_back();
		if (l > max_length)
			max_length = l;
	}

	if ((doors & UP) && y > 1)
	{
		path.push_back('U');
		auto l = SearchMax(x, y - 1, length + 1, path);
		path.pop_back();
		if (l > max_length)
			max_length = l;
	}

	if ((doors & LEFT) && x > 1)
	{
		path.push_back('L');
		auto l = SearchMax(x - 1, y, length + 1, path);
		path.pop_back();
		if (l > max_length)
			max_length = l;
	}

	return max_length;
}

std::string Solve(std::string prefix)
{
	std::string min_path;
	Search(1, 1, 0, prefix, min_path);
	return min_path.substr(prefix.size());
}

int Solve2(std::string prefix)
{
	return SearchMax(1, 1, 0, prefix);
}

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
	"2016-17"_test = [] {
		expect(eq((UP | DOWN | LEFT), GetDoors("hijkl")));
		expect(eq((UP | LEFT | RIGHT), GetDoors("hijklD")));
		expect(eq(RIGHT, GetDoors("hijklDU")));

		expect(eq("DDRRRD"s, Solve("ihgpwlah")));
		expect(eq("DDUDRLRRUDRD"s, Solve("kglvqrro")));
		expect(eq("DRURDRUDDLLDLUURRDULRLDUUDDDRR"s, Solve("ulqzkmiv")));

		Printer::Print(__FILE__, "1", Solve("gdjjyniy"));

		expect(370_i == Solve2("ihgpwlah"));
		expect(492_i == Solve2("kglvqrro"));
		expect(830_i == Solve2("ulqzkmiv"));

		Printer::Print(__FILE__, "2", Solve2("gdjjyniy"));
	};
};

} //namespace;