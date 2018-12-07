#pragma once

#include <vector>
#include <numeric>
#include <string>

class Circle
{
public:
	std::vector<unsigned> _buf;

	Circle(unsigned size)
		: _buf(size)
	{
		std::iota(_buf.begin(), _buf.end(), 0);
	}

	void Reverse(unsigned i, unsigned j)
	{
		for (--j; i < j; ++i, --j)
		{
			std::swap(_buf[i % _buf.size()], _buf[j % _buf.size()]);
		}
	}

	unsigned GetAt(unsigned i) const
	{
		return _buf[i % _buf.size()];
	}
};

inline std::vector<uint8_t> KnotHash(const std::string &input)
{
	std::vector<unsigned> lengths;
	for (char ch : input)
		lengths.push_back(ch);
	for (unsigned t : {17, 31, 73, 47, 23})
		lengths.push_back(t);

	Circle circle(256);
	unsigned position{0};
	unsigned skip{0};

	for (int i = 0; i < 64; ++i)
	{
		for (unsigned l : lengths)
		{
			circle.Reverse(position, position + l);
			position += l + skip++;
			position %= 256;
		}
	}

	std::vector<uint8_t> hash;
	for (int i = 0; i < 16; ++i)
	{
		unsigned x = std::accumulate(circle._buf.begin() + i*16,
									 circle._buf.begin() + (i+1)*16,
									 0,
									 [](unsigned a, unsigned b) { return a ^ b; });
		hash.push_back(x);
	}

	return hash;
}

