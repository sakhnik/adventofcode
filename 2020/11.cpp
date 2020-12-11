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

    Seats(const Seats &s)
        : _seats{s._seats}
        , _cols{s._cols}
        , _rows{s._rows}
    {
    }

    void Run(int count = std::numeric_limits<int>::max())
    {
        _Run(count, [this](int r, int c) { return _CountNeighbours(r, c); }, 4);
    }

    void Run2(int count = std::numeric_limits<int>::max())
    {
        _Run(count, [this](int r, int c) { return _CountDirections(r, c); }, 5);
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

    void _Run(int count, std::function<int(int, int)> counter, int tolerance)
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
                    int neighbours = counter(row, col);
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
                        if (neighbours >= tolerance)
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

    int _CountDirections(int row, int col) const
    {
        int count{};
        for (int dr = -1; dr <= 1; ++dr)
        {
            for (int dc = -1; dc <= 1; ++dc)
            {
                if (dr || dc)
                    count += _CountOneDirection(row, col, dr, dc);
            }
        }
        return count;
    }

    int _CountOneDirection(int row, int col, int dr, int dc) const
    {
        row += dr;
        col += dc;
        switch (_Get(row, col))
        {
        case 'L': return 0;
        case '#': return 1;
        }
        return _CountOneDirection(row, col, dr, dc);
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
            Seats s{std::istringstream{TEST}};

            s.Run2(1);
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
            expect(eq(s.GetSeats(), ROUND1));

            s.Run2(1);
            const std::string ROUND2 =
                "#.LL.LL.L#\n"
                "#LLLLLL.LL\n"
                "L.L.L..L..\n"
                "LLLL.LL.LL\n"
                "L.LL.LL.LL\n"
                "L.LLLLL.LL\n"
                "..L.L.....\n"
                "LLLLLLLLL#\n"
                "#.LLLLLL.L\n"
                "#.LLLLL.L#";
            expect(eq(s.GetSeats(), ROUND2));
        }

        Seats seats{std::ifstream{INPUT}};
        Seats seats2{seats};

        seats.Run();
        Printer::Print(__FILE__, "1", seats.CountOccupied());

        seats2.Run2();
        Printer::Print(__FILE__, "2", seats2.CountOccupied());
    };
};

} //namespace;
