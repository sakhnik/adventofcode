#include <doctest/doctest.h>
#include <fstream>
#include <iostream>
#include <vector>


TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    std::string pix;
    std::getline(ifs, pix);

    size_t layer_size = 25 * 6;
    CHECK(pix.size() % layer_size == 0);
    size_t layer_count = pix.size() / layer_size;

    std::vector<int> counts(10, std::numeric_limits<int>::max()), s(10, 0);
    for (size_t i = 0; i < layer_count; ++i)
    {
        std::fill(begin(s), end(s), 0);
        for (size_t j{layer_size * i}, jn{layer_size * (i+1)};
             j < jn; ++j)
        {
            ++s[pix[j] - '0'];
        }
        if (s[0] < counts[0])
        {
            counts.swap(s);
        }
    }

    MESSAGE(counts[1] * counts[2]);
}
