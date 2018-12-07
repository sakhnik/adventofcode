#include <doctest/doctest.h>
#include <iostream>
#include <limits>
#include <cstring>
#include <cassert>
#include <openssl/md5.h>

using namespace std;

string Solve(const char *prefix)
{
	string ret;

	char buffer[64];
	int end_of_prefix = sprintf(buffer, "%s", prefix);

	for (int i = 0; i < std::numeric_limits<int>::max(); ++i)
	{
		int len = end_of_prefix + sprintf(buffer + end_of_prefix, "%d", i);

		unsigned char md5[MD5_DIGEST_LENGTH];
		MD5(reinterpret_cast<const uint8_t *>(buffer), len, md5);

		if (!md5[0] && !md5[1] && !(md5[2] & 0xf0))
		{
			static char digits[] = "0123456789abcdef";
			ret.push_back(digits[md5[2] & 0xf]);
			if (ret.size() == 8)
				return ret;
		}
	}

	return "?";
}

string Solve2(const char *prefix)
{
	string ret(8, ' ');

	char buffer[64];
	int end_of_prefix = sprintf(buffer, "%s", prefix);

	for (int i = 0; i < std::numeric_limits<int>::max(); ++i)
	{
		int len = end_of_prefix + sprintf(buffer + end_of_prefix, "%d", i);

		unsigned char md5[MD5_DIGEST_LENGTH];
		MD5(reinterpret_cast<const uint8_t *>(buffer), len, md5);

		if (!md5[0] && !md5[1] && !(md5[2] & 0xf0))
		{
			static char digits[] = "0123456789abcdef";
			int position = md5[2] & 0xf;
			if (position < 8 && ret[position] == ' ')
			{
				char ch = digits[(md5[3] & 0xf0) >> 4];
				ret[position] = ch;
				if (ret.npos == ret.find_first_of(' '))
					return ret;
			}
		}
	}

	return "?";
}

TEST_CASE(TEST_NAME)
{
	REQUIRE("18f47a30" == Solve("abc"));
	cout << Solve("reyedfim") << endl;

	REQUIRE("05ace8e3" == Solve2("abc"));
	cout << Solve2("reyedfim") << endl;
}

// vim: set makeprg=clang++\ -std=c++14\ %\ -lssl\ -lcrypto:
