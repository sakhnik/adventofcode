#include "../test.hpp"
#include <boost/functional/hash.hpp>

namespace {

using namespace boost::ut;

using IntT = uint64_t;

struct Stone
{
    IntT value;
    int level;

    bool operator==(const Stone &o) const = default;

    struct Hash
    {
        size_t operator()(const Stone &s) const {
            size_t r{};
            boost::hash_combine(r, s.level);
            boost::hash_combine(r, s.value);
            return r;
        }
    };
};

class Task
{
    std::unordered_map<Stone, IntT, Stone::Hash> counts;

public:
    Task(std::istream &&is)
    {
        std::copy(std::istream_iterator<int>(is), std::istream_iterator<int>(), std::back_inserter(nums));
    }

    IntT Blink(int count)
    {
        IntT res{};
        for (auto n : nums) {
            res += Blink(n, count);
        }
        return res;
    }

    IntT Blink(IntT n, int count)
    {
        if (count == 0)
            return 1;
        Stone stone{n, count};
        auto it = counts.find(stone);
        if (it != counts.end())
            return it->second;
        if (n == 0)
            return counts[stone] = Blink(1, count - 1);
        auto s = std::to_string(n);
        if (0 == (s.size() & 1)) {
            return counts[stone] = Blink(std::stoull(s.substr(0, s.size() / 2)), count - 1)
                + Blink(std::stoull(s.substr(s.size() / 2)), count - 1);
        }
        return counts[stone] = Blink(n * 2024, count - 1);
    }

private:
    std::vector<IntT> nums;
};

suite s = [] {
    "11"_test = [] {
        const char *const TEST1 = R"(125 17)";
        auto test = Task(std::istringstream{TEST1});
        expect(22_i == test.Blink(6));
        expect(55312_i == test.Blink(25));

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.Blink(25));
        Printer::Print(__FILE__, "2", task.Blink(75));
    };
};

} //namespace;
