#include <cassert>
#include <boost/range/adaptor/reversed.hpp>
#include "../test.hpp"

namespace {

std::string Checksum(const std::string &s)
{
	if (s.size() & 1)
		return s;
	std::string cs;
	for (unsigned i = 0; i != s.size(); i += 2)
		cs.push_back(s[i] == s[i+1] ? '1' : '0');
	return Checksum(cs);
}

std::string Generate(const std::string &s, unsigned size)
{
	if (s.size() >= size)
		return s.substr(0, size);
	std::string es;
	for (char c : s)
		es.push_back(c);
	es.push_back('0');
	for (char c : boost::adaptors::reverse(s))
		es.push_back(c == '0' ? '1' : '0');
	return Generate(es, size);
}

std::string Solve(unsigned size, const std::string &init)
{
	auto data = Generate(init, size);
	return Checksum(data);
}

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
	"16"_test = [] {
		expect(eq("100"s, Checksum("110010110100")));
		expect(eq("100"s, Generate("1", 3)));
		expect(eq("001"s, Generate("0", 3)));
		expect(eq("11111000000"s, Generate("11111", 11)));
		expect(eq("1111000010100101011110000"s, Generate("111100001010", 25)));
		expect(eq("01100"s, Solve(20, "10000")));

		if (Printer::EnableAll())
		{
			Printer::Print(__FILE__, "1", Solve(272, "10011111011011001"));
			Printer::Print(__FILE__, "2", Solve(35651584, "10011111011011001"));
		}
	};
};

} //namespace;
