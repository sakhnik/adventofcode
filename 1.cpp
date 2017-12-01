#include <iostream>
#include <string_view>
#include <cassert>

unsigned Sum(std::string_view s)
{
	unsigned sum{0};

	for (size_t i = 0, n = s.size() - 1; i != n; ++i)
	{
		if (s[i] == s[i+1])
			sum += s[i] - '0';
	}

	if (s.front() == s.back())
		sum += s.front() - '0';

	return sum;
}

int main()
{
	assert(Sum("1122") == 3);
	assert(Sum("1111") == 4);
	assert(Sum("1234") == 0);
	assert(Sum("91212129") == 9);

	std::string input;
	getline(std::cin, input);
	std::cout << Sum(input) << std::endl;
}
