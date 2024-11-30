#include "../test.hpp"
#include <fstream>

namespace {

std::pair<size_t, size_t> FindLargestCalories(std::istream &&ifs)
{
    // min priority queue
    std::priority_queue<size_t, std::vector<size_t>, std::greater<size_t>> pq;

    size_t calories{};
    std::string line;
    while (getline(ifs, line))
    {
        if (line.empty())
        {
            pq.push(calories);
            if (pq.size() > 3)
                pq.pop();
            calories = 0;
            continue;
        }

        calories += std::stoi(line);
    }
    pq.push(calories);
    if (pq.size() > 3)
        pq.pop();

    size_t total{pq.top()};
    pq.pop();
    total += pq.top();
    pq.pop();
    total += pq.top();
    return {pq.top(), total};
}

const char *const TEST = R"(1000
2000
3000

4000

5000
6000

7000
8000
9000

10000
)";

using namespace boost::ut;

suite s = [] {
    "01"_test = [] {
        auto test_res = FindLargestCalories(std::istringstream{TEST});
        expect(24000_u == test_res.first);
        expect(45000_u == test_res.second);

        std::ifstream ifs{INPUT};
        auto res = FindLargestCalories(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", res.first);
        Printer::Print(__FILE__, "2", res.second);
    };
};

} //namespace;
