#include "../test.hpp"
#include <unordered_map>
#include <fstream>
#include <numeric>
#include <list>
#include <iterator>

namespace {

class Board
{
public:
    Board(std::istream &is)
        : _rows(5, 5)
        , _cols(5, 5)
    {
        int num{};
        for (int row = 0; row < 5; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                if (!(is >> num))
                {
                    throw std::runtime_error("EOD");
                }
                _numbers[num] = {row, col};
            }
        }
    }

    bool Mark(int number)
    {
        auto it = _numbers.find(number);
        if (it == _numbers.end())
            return false;
        auto row_col = it->second;
        _numbers.erase(it);
        if (0 == --_rows[row_col.row])
            return true;
        if (0 == --_cols[row_col.col])
            return true;
        return false;
    }

    ssize_t Score() const
    {
        return std::accumulate(_numbers.begin(), _numbers.end(), 0ull,
                [](ssize_t a, const auto &n) {
                    return a + n.first;
                });
    }

private:
    struct RowCol
    {
        int row{}, col{};
    };
    std::unordered_map<int, RowCol> _numbers;
    std::vector<int> _rows;
    std::vector<int> _cols;
};

class Bingo
{
public:
    Bingo(std::istream &&is)
    {
        std::string line;
        is >> line;
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream iss{line};
        _numbers.assign(std::istream_iterator<int>{iss}, std::istream_iterator<int>{});

        try
        {
            while (is)
                _boards.emplace_back(is);
        }
        catch (std::exception &)
        {
        }
    }

    std::pair<ssize_t, ssize_t> Play()
    {
        ssize_t first_winner{};
        ssize_t last_winner{};

        for (auto n : _numbers)
        {
            for (auto it = _boards.begin(); it != _boards.end(); )
            {
                if (it->Mark(n))
                {
                    // Bingo!
                    if (!first_winner)
                        first_winner = it->Score() * n;
                    if (_boards.size() == 1)
                    {
                        last_winner = it->Score() * n;
                        return {first_winner, last_winner};
                    }
                    it = _boards.erase(it);
                }
                else
                    ++it;
            }
        }

        return {first_winner, last_winner};
    }

private:
    std::vector<int> _numbers;
    std::list<Board> _boards;
};

using namespace boost::ut;

const char *const TEST_INPUT = R"(
7,4,9,5,11,17,23,2,0,14,21,24,10,16,13,6,15,25,12,22,18,20,8,19,3,26,1

22 13 17 11  0
 8  2 23  4 24
21  9 14 16  7
 6 10  3 18  5
 1 12 20 15 19

 3 15  0  2 22
 9 18 13 17  5
19  8  7 25 23
20 11 10 24  4
14 21 16 12  6

14 21 17 24  4
10 16 15  9 19
18  8 23 26 20
22 11 13  6  5
 2  0 12  3  7
)";

suite s = [] {
    "04"_test = [] {
        Bingo test{std::istringstream{TEST_INPUT}};
        auto winners = test.Play();
        expect(4512_i == winners.first);
        expect(1924_i == winners.second);

        Bingo bingo{std::ifstream{INPUT}};
        winners = bingo.Play();
        Printer::Print(__FILE__, "1", winners.first);
        Printer::Print(__FILE__, "2", winners.second);
    };
};

} //namespace;
