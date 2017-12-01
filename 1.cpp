#include <iostream>
#include <string_view>
#include <cassert>

unsigned Sum(std::string_view s, unsigned dist)
{
	unsigned sum{0};

	for (size_t i = 0, n = s.size(); i != n; ++i)
	{
		if (s[i] == s[(i+dist) % s.size()])
			sum += s[i] - '0';
	}

	return sum;
}

int main()
{
	assert(Sum("1122", 1) == 3);
	assert(Sum("1111", 1) == 4);
	assert(Sum("1234", 1) == 0);
	assert(Sum("91212129", 1) == 9);
	assert(Sum("1212", 2) == 6);
	assert(Sum("1221", 2) == 0);
	assert(Sum("123425", 3) == 4);
	assert(Sum("123123", 3) == 12);
	assert(Sum("12131415", 4) == 4);

	std::string input;
	getline(std::cin, input);
	std::cout << Sum(input, 1) << std::endl;
	std::cout << Sum(input, input.size() / 2) << std::endl;
}
