#include "../test.hpp"
#include <streambuf>
#include <numeric>
#include <sstream>
#include <fstream>
#include <limits>

namespace {

class Seats
{
public:
    Seats(std::istream &&is)
        : _seats((std::istreambuf_iterator<char>{is}),
                  std::istreambuf_iterator<char>{})
    {
        _cols = _seats.find('\n');
        _rows = (_seats.size() + 1) / (_cols + 1);
    }

    void Run(int count = std::numeric_limits<int>::max())
    {
        do
        {
            std::string buf{_seats};
            bool change{false};

            for (int row = 0; row < _rows; ++row)
            {
                for (int col = 0; col < _cols; ++col)
                {
                    char cur = _Get(row, col);
                    int neighbours = _CountNeighbours(row, col);
                    if (cur == 'L')
                    {
                        if (neighbours == 0)
                        {
                            buf[row * (_cols + 1) + col] = '#';
                            change = true;
                        }
                    }
                    else if (cur == '#')
                    {
                        if (neighbours >= 4)
                        {
                            buf[row * (_cols + 1) + col] = 'L';
                            change = true;
                        }
                    }
                }
            }

            if (!change)
                break;

            _seats.swap(buf);
        }
        while (--count > 0);
    }

    size_t CountOccupied() const
    {
        return std::count(_seats.begin(), _seats.end(), '#');
    }

    const std::string& GetSeats() const { return _seats; }

private:
    std::string _seats;
    int _cols;
    int _rows;

    int _CountNeighbours(int row, int col) const
    {
        return (_Get(row - 1, col - 1) == '#')
            + (_Get(row - 1, col + 0) == '#')
            + (_Get(row - 1, col + 1) == '#')
            + (_Get(row + 0, col - 1) == '#')
            + (_Get(row + 0, col + 1) == '#')
            + (_Get(row + 1, col - 1) == '#')
            + (_Get(row + 1, col + 0) == '#')
            + (_Get(row + 1, col + 1) == '#');
    }

    char _Get(int row, int col) const
    {
        if (row < 0 || row >= _rows)
            return 'L';
        if (col < 0 || col >= _cols)
            return 'L';
        return _seats[row * (_cols + 1) + col];
    }
};

using namespace boost::ut;

suite s = [] {
    "2020-11"_test = [] {
        {
            const char *const TEST =
                "L.LL.LL.LL\n"
                "LLLLLLL.LL\n"
                "L.L.L..L..\n"
                "LLLL.LL.LL\n"
                "L.LL.LL.LL\n"
                "L.LLLLL.LL\n"
                "..L.L.....\n"
                "LLLLLLLLLL\n"
                "L.LLLLLL.L\n"
                "L.LLLLL.LL";
            Seats seats{std::istringstream{TEST}};

            seats.Run(1);
            const std::string ROUND1 =
                "#.##.##.##\n"
                "#######.##\n"
                "#.#.#..#..\n"
                "####.##.##\n"
                "#.##.##.##\n"
                "#.#####.##\n"
                "..#.#.....\n"
                "##########\n"
                "#.######.#\n"
                "#.#####.##";
            expect(eq(seats.GetSeats(), ROUND1));

            seats.Run(1);
            const std::string ROUND2 =
                "#.LL.L#.##\n"
                "#LLLLLL.L#\n"
                "L.L.L..L..\n"
                "#LLL.LL.L#\n"
                "#.LL.LL.LL\n"
                "#.LLLL#.##\n"
                "..L.L.....\n"
                "#LLLLLLLL#\n"
                "#.LLLLLL.L\n"
                "#.#LLLL.##";
            expect(eq(seats.GetSeats(), ROUND2));

            seats.Run(1);
            const std::string ROUND3 =
                "#.##.L#.##\n"
                "#L###LL.L#\n"
                "L.#.#..#..\n"
                "#L##.##.L#\n"
                "#.##.LL.LL\n"
                "#.###L#.##\n"
                "..#.#.....\n"
                "#L######L#\n"
                "#.LL###L.L\n"
                "#.#L###.##";
            expect(eq(seats.GetSeats(), ROUND3));

            seats.Run(1);
            const std::string ROUND4 =
                "#.#L.L#.##\n"
                "#LLL#LL.L#\n"
                "L.L.L..#..\n"
                "#LLL.##.L#\n"
                "#.LL.LL.LL\n"
                "#.LL#L#.##\n"
                "..L.L.....\n"
                "#L#LLLL#L#\n"
                "#.LLLLLL.L\n"
                "#.#L#L#.##";
            expect(eq(seats.GetSeats(), ROUND4));

            seats.Run(1);
            const std::string ROUND5 =
                "#.#L.L#.##\n"
                "#LLL#LL.L#\n"
                "L.#.L..#..\n"
                "#L##.##.L#\n"
                "#.#L.LL.LL\n"
                "#.#L#L#.##\n"
                "..L.L.....\n"
                "#L#L##L#L#\n"
                "#.LLLLLL.L\n"
                "#.#L#L#.##";
            expect(eq(seats.GetSeats(), ROUND5));

            seats.Run();
            expect(37_u == seats.CountOccupied());
        }

        Seats seats{std::ifstream{INPUT}};
        seats.Run(70);
        Printer::Print(__FILE__, "1", seats.CountOccupied());
    };
};

} //namespace;
