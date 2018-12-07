#include <doctest/doctest.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

using namespace std;

bool IsAbba(const char *s)
{
	return s[0] == s[3] && s[1] == s[2] && s[0] != s[1];
}

bool IsTls(const string &addr)
{
	bool is_hyper{false};
	bool is_tls{false};

	for (size_t i = 0; i != addr.size() - 3; ++i)
	{
		switch (addr[i])
		{
		case '[':
			assert(!is_hyper);
			is_hyper = true;
			break;
		case ']':
			assert(is_hyper);
			is_hyper = false;
			break;
		default:
			if (IsAbba(addr.c_str() + i))
			{
				if (is_hyper)
					return false;
				is_tls = true;
			}
		}
	}

	return is_tls;
}

bool FindHyper(const string &addr, char a, char b)
{
	bool is_hyper{false};

	for (size_t i = 0; i != addr.size() - 2; ++i)
	{
		switch (addr[i])
		{
		case '[':
			is_hyper = true;
			break;
		case ']':
			is_hyper = false;
			break;
		default:
			if (is_hyper)
			{
				if (addr[i] == b && addr[i+1] == a && addr[i+2] == b)
					return true;
			}

		}
	}

	return false;
}

bool IsSsl(const string &addr)
{
	bool is_hyper{false};

	for (size_t i = 0; i != addr.length() - 2; ++i)
	{
		switch (addr[i])
		{
		case '[':
			assert(!is_hyper);
			is_hyper = true;
			break;
		case ']':
			assert(is_hyper);
			is_hyper = false;
			break;
		default:
			if (!is_hyper)
			{
				if (addr[i] == addr[i+2] && addr[i] != addr[i+1])
				{
					if (FindHyper(addr, addr[i], addr[i+1]))
						return true;
				}
			}
		}
	}

	return false;
}

template <typename FuncT>
int Solve(istream &&is, FuncT func)
{
	int count{0};

	string line;
	while (getline(is, line))
	{
		if (func(line))
			++count;
	}

	return count;
}

TEST_CASE(TEST_NAME)
{
	REQUIRE(IsTls("abba[mnop]qrst"));
	REQUIRE(!IsTls("abcd[bddb]xyyx"));
	REQUIRE(!IsTls("aaaa[qwer]tyui"));
	REQUIRE(IsTls("ioxxoj[asdfgh]zxcvbn"));

	cout << Solve(ifstream(INPUT), IsTls) << endl;

	REQUIRE(IsSsl("aba[bab]xyz"));
	REQUIRE(!IsSsl("xyx[xyx]xyx"));
	REQUIRE(IsSsl("aaa[kek]eke"));
	REQUIRE(IsSsl("zazbz[bzb]cdb"));

	cout << Solve(ifstream(INPUT), IsSsl) << endl;
}
