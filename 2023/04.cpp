#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

class Game
{
    std::vector<int> wins;

public:
    Game(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            std::istringstream iss{line};
            std::string token;
            iss >> token >> token;

            std::vector<int> winning;
            auto is_winning = [&winning](int num) {
                return std::find(winning.begin(), winning.end(), num) != winning.end();
            };

            while (iss >> token)
            {
                if (token == "|")
                    break;
                winning.push_back(std::stoi(token));
            }

            int win_count{};
            while (iss >> token)
            {
                if (is_winning(std::stoi(token)))
                    ++win_count;
            }
            wins.push_back(win_count);
        }
    }

    uint32_t GetTotalPoints() const
    {
        uint32_t points{};
        for (int w : wins)
            points += w ? (1ul << (w - 1)) : 0;
        return points;
    }

    uint64_t GetTotalCopies() const
    {
        std::vector<uint64_t> counts(wins.size(), 1);
        for (size_t i = 0; i < counts.size(); ++i)
        {
            for (int j = 1, jN = wins[i]; j <= jN && j < counts.size(); ++j)
            {
                counts[i + j] += counts[i];
            }
        }
        return std::accumulate(counts.begin(), counts.end(), uint64_t{0});
    }
};

suite s = [] {
    "2023-04"_test = [] {
        const char *const TEST1 = R"(Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19
Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1
Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83
Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36
Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11
)";
        Game test_game{std::istringstream{TEST1}};
        expect(13_u == test_game.GetTotalPoints());
        expect(30_u == test_game.GetTotalCopies());

        Game game{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", game.GetTotalPoints());
        Printer::Print(__FILE__, "2", game.GetTotalCopies());
    };
};

} //namespace;
