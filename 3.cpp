#include <iostream>
#include <cassert>

unsigned CountUntil(unsigned n)
{
	int x{0}, y{0}, a{0};
	unsigned c{1};

	auto dist = [](int x, int y) {
		return (x > 0 ? x : -x) + (y > 0 ? y : -y);
	};

	while (true)
	{
		++x;
		a += 1;
		if (++c == n)
			return dist(x, y);
		while (y < a)
		{
			++y;
			if (++c == n)
				return dist(x, y);
		}
		while (x > -a)
		{
			--x;
			if (++c == n)
				return dist(x, y);
		}
		while (y > -a)
		{
			--y;
			if (++c == n)
				return dist(x, y);
		}
		while (x < a)
		{
			++x;
			if (++c == n)
				return dist(x, y);
		}
	}
}

int main()
{
	assert(CountUntil(2) == 1);
	assert(CountUntil(3) == 2);
	assert(CountUntil(4) == 1);
	assert(CountUntil(5) == 2);
	assert(CountUntil(12) == 3);
	assert(CountUntil(23) == 2);
	assert(CountUntil(1024) == 31);

	std::cout << CountUntil(289326) << std::endl;
}
