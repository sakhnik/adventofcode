#include <fstream>
#include <vector>
#include <limits>
#include "../test.hpp"
namespace {

using namespace boost::ut;

suite s = [] {
    "2019-08"_test = [] {
        const size_t WIDTH = 25;
        const size_t HEIGHT = 6;

        std::ifstream ifs{INPUT};
        std::string pix;
        std::getline(ifs, pix);

        size_t layer_size = WIDTH * HEIGHT;
        expect(pix.size() % layer_size == 0_u);
        size_t layer_count = pix.size() / layer_size;

        std::vector<int> counts(10, std::numeric_limits<int>::max()), s(10, 0);
        for (size_t i = 0; i < layer_count; ++i)
        {
            std::fill(begin(s), end(s), 0);
            for (size_t j{layer_size * i}, jn{layer_size * (i + 1)};
                 j < jn; ++j)
            {
                ++s[pix[j] - '0'];
            }
            if (s[0] < counts[0])
            {
                counts.swap(s);
            }
        }

        Printer::Print(__FILE__, "1", counts[1] * counts[2]);

        // Merge the layers from bottom to top
        for (size_t i = 0; i + 1 < layer_count; ++i)
        {
            for (size_t j{layer_size * (layer_count - i - 2)}, jn{j + layer_size};
                 j < jn; ++j)
            {
                if (pix[j] == '2')
                {
                    pix[j] = pix[j + layer_size];
                }
            }
        }

        std::ostringstream oss;
        oss << "\n";
        // Display nicely
        for (size_t r{0}; r < HEIGHT; ++r)
        {
            for (size_t c{r * WIDTH}, cn{c + WIDTH}; c < cn; ++c)
            {
                if (pix[c] == '0')
                {
                    oss << ' ';
                }
                else if (pix[c] == '1')
                {
                    oss << '#';
                }
            }
            oss << '\n';
        }
        Printer::Print(__FILE__, "2", oss.str());
    };
};

} //namespace;