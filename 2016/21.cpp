#include <doctest/doctest.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <vector>

namespace {

void Swap(std::string &s, int X, int Y)
{
	std::swap(s[X], s[Y]);
}

void Swap(std::string &s, char A, char B)
{
	std::swap(s[s.find(A)], s[s.find(B)]);
}

void RoL(std::string &s, int X)
{
	std::rotate(begin(s), begin(s) + X % s.size(), end(s));
}

void RoR(std::string &s, int X)
{
	int idx = ((int)s.size() - X) % (int)s.size();
	if (idx < 0)
		idx += s.size();
	rotate(begin(s), begin(s) + idx, end(s));
}

void RoB(std::string &s, char A)
{
	int idx = s.find(A);
	if (idx >= 4)
		++idx;
	++idx;

	idx = ((int)s.size() - idx) % (int)s.size();
	if (idx < 0)
		idx += s.size();

	rotate(begin(s), begin(s) + idx, end(s));
}

void Reverse(std::string &s, int X, int Y)
{
	reverse(begin(s) + X, begin(s) + Y + 1);
}

void Move(std::string &s, int X, int Y)
{
	char c = s[X];
	s.erase(s.begin() + X);
	if (Y >= (int)s.size())
		s.push_back(c);
	else
		s.insert(begin(s) + Y, c);
}

std::string Scramble(std::istream &&is, std::string s)
{
	std::string line;
	while (getline(is, line))
	{
		int X{}, Y{};
		char A{}, B{};
		if (2 == sscanf(line.c_str(), "swap position %d with position %d", &X, &Y))
			Swap(s, X, Y);
		else if (2 == sscanf(line.c_str(), "swap letter %c with letter %c", &A, &B))
			Swap(s, A, B);
		else if (1 == sscanf(line.c_str(), "rotate left %d steps", &X))
			RoL(s, X);
		else if (1 == sscanf(line.c_str(), "rotate right %d steps", &X))
			RoR(s, X);
		else if (1 == sscanf(line.c_str(), "rotate based on position of letter %c", &A))
			RoB(s, A);
		else if (2 == sscanf(line.c_str(), "reverse positions %d through %d", &X, &Y))
			Reverse(s, X, Y);
		else if (2 == sscanf(line.c_str(), "move position %d to position %d", &X, &Y))
			Move(s, X, Y);
	}

	return s;
}

std::string Unscramble(std::istream &&is, std::string s)
{
	std::vector<std::string> lines;

	std::string line;
	while (getline(is, line))
	{
		lines.push_back(line);
	}

	for (; !lines.empty(); lines.pop_back())
	{
		const auto &line = lines.back();

		int X{}, Y{};
		char A{}, B{};
		if (2 == sscanf(line.c_str(), "swap position %d with position %d", &X, &Y))
			Swap(s, X, Y);
		else if (2 == sscanf(line.c_str(), "swap letter %c with letter %c", &A, &B))
			Swap(s, A, B);
		else if (1 == sscanf(line.c_str(), "rotate right %d steps", &X))
			RoL(s, X);
		else if (1 == sscanf(line.c_str(), "rotate left %d steps", &X))
			RoR(s, X);
		else if (1 == sscanf(line.c_str(), "rotate based on position of letter %c", &A))
		{
			// Brute force initial position
			for (int i = 0, n = s.size(); i != n; ++i)
			{
				std::string s1(s);
				RoL(s1, i);
				RoB(s1, A);
				if (s1 == s)
				{
					RoL(s, i);
					break;
				}
			}
		}
		else if (2 == sscanf(line.c_str(), "reverse positions %d through %d", &X, &Y))
			Reverse(s, X, Y);
		else if (2 == sscanf(line.c_str(), "move position %d to position %d", &Y, &X))
			Move(s, X, Y);
	}

	return s;
}

} //namespace;

TEST_CASE(TEST_NAME)
{
	REQUIRE("ebcda" == Scramble(std::istringstream{"swap position 4 with position 0"}, "abcde"));
	REQUIRE("edcba" == Scramble(std::istringstream{"swap letter d with letter b"}, "ebcda"));
	REQUIRE("dabc" == Scramble(std::istringstream{"rotate right 1 steps"}, "abcd"));
	REQUIRE("bcdea" == Scramble(std::istringstream{"rotate left 1 steps"}, "abcde"));
	REQUIRE("ecabd" == Scramble(std::istringstream{"rotate based on position of letter b"}, "abdec"));
	REQUIRE("decab" == Scramble(std::istringstream{"rotate based on position of letter d"}, "ecabd"));
	REQUIRE("abcde" == Scramble(std::istringstream{"reverse positions 0 through 4"}, "edcba"));
	REQUIRE("bdeac" == Scramble(std::istringstream{"move position 1 to position 4"}, "bcdea"));
	REQUIRE("abdec" == Scramble(std::istringstream{"move position 3 to position 0"}, "bdeac"));

	MESSAGE(Scramble(std::ifstream{INPUT}, "abcdefgh"));

	REQUIRE("abcde" == Unscramble(std::istringstream{"swap position 4 with position 0"}, "ebcda"));
	REQUIRE("ebcda" == Unscramble(std::istringstream{"swap letter d with letter b"}, "edcba"));
	REQUIRE("abcd" == Unscramble(std::istringstream{"rotate right 1 steps"}, "dabc"));
	REQUIRE("abcde" == Unscramble(std::istringstream{"rotate left 1 steps"}, "bcdea"));
	REQUIRE("abdec" == Unscramble(std::istringstream{"rotate based on position of letter b"}, "ecabd"));
	REQUIRE("ecabd" == Unscramble(std::istringstream{"rotate based on position of letter d"}, "decab"));
	REQUIRE("edcba" == Unscramble(std::istringstream{"reverse positions 0 through 4"}, "abcde"));
	REQUIRE("bcdea" == Unscramble(std::istringstream{"move position 1 to position 4"}, "bdeac"));
	REQUIRE("bdeac" == Unscramble(std::istringstream{"move position 3 to position 0"}, "abdec"));

	MESSAGE(Unscramble(std::ifstream{INPUT}, "fbgdceah"));
}
