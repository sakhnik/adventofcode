#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <boost/ut.hpp>

namespace {

bool IsAbba(const char *s)
{
	return s[0] == s[3] && s[1] == s[2] && s[0] != s[1];
}

bool IsTls(const std::string &addr)
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

bool FindHyper(const std::string &addr, char a, char b)
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

bool IsSsl(const std::string &addr)
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
int Solve(std::istream &&is, FuncT func)
{
	int count{0};

	std::string line;
	while (getline(is, line))
	{
		if (func(line))
			++count;
	}

	return count;
}

using namespace boost::ut;

suite s = [] {
	"2016-07"_test = [] {
		expect(IsTls("abba[mnop]qrst"));
		expect(!IsTls("abcd[bddb]xyyx"));
		expect(!IsTls("aaaa[qwer]tyui"));
		expect(IsTls("ioxxoj[asdfgh]zxcvbn"));

		std::cout << "2016-07.1: " << Solve(std::ifstream{INPUT}, IsTls) << std::endl;

		expect(IsSsl("aba[bab]xyz"));
		expect(!IsSsl("xyx[xyx]xyx"));
		expect(IsSsl("aaa[kek]eke"));
		expect(IsSsl("zazbz[bzb]cdb"));

		std::cout << "2016-07.2: " << Solve(std::ifstream{INPUT}, IsSsl) << std::endl;
	};
};

} //namespace;