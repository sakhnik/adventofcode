#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <iostream>
#include <cassert>
#include <boost/range/adaptor/reversed.hpp>

using namespace std;

string Checksum(const string &s)
{
	if (s.size() & 1)
		return s;
	string cs;
	for (unsigned i = 0; i != s.size(); i += 2)
		cs.push_back(s[i] == s[i+1] ? '1' : '0');
	return Checksum(cs);
}

string Generate(const string &s, unsigned size)
{
	if (s.size() >= size)
		return s.substr(0, size);
	string es;
	for (char c : s)
		es.push_back(c);
	es.push_back('0');
	for (char c : boost::adaptors::reverse(s))
		es.push_back(c == '0' ? '1' : '0');
	return Generate(es, size);
}

string Solve(unsigned size, const string &init)
{
	auto data = Generate(init, size);
	return Checksum(data);
}

TEST_CASE("main")
{
	REQUIRE("100" == Checksum("110010110100"));
	REQUIRE("100" == Generate("1", 3));
	REQUIRE("001" == Generate("0", 3));
	REQUIRE("11111000000" == Generate("11111", 11));
	REQUIRE("1111000010100101011110000" == Generate("111100001010", 25));
	REQUIRE("01100" == Solve(20, "10000"));

	cout << Solve(272, "10011111011011001") << endl;
	cout << Solve(35651584, "10011111011011001") << endl;
}
