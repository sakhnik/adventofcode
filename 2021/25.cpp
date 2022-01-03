#include "../test.hpp"

namespace {

class Cucumber
{
public:
    using MapT = std::vector<std::string>;
    MapT map;

    Cucumber(std::istream &&is)
    {
        std::string line;
        while (is >> line)
            map.push_back(std::move(line));
    }

    std::string Dump() const
    {
        std::ostringstream oss;
        std::string nl = "";
        for (const auto &l : map)
        {
            oss << nl << l;
            nl = "\n";
        }
        return oss.str();
    }

    bool Step()
    {
        bool moved{false};

        for (size_t row = 0; row < map.size(); ++row)
        {
            bool first_vacant = map[row][0] == '.';
            for (size_t col = 0; col < map[row].size(); ++col)
            {
                if (map[row][col] != '>')
                    continue;
                size_t next_col = col + 1;
                if (next_col >= map[row].size())
                {
                    if (!first_vacant)
                        continue;
                    next_col = 0;
                }
                if (map[row][next_col] != '.')
                    continue;
                map[row][col] = '.';
                map[row][next_col] = '>';
                ++col;
                moved = true;
            }
        }

        for (size_t col = 0; col < map[0].size(); ++col)
        {
            bool first_vacant = map[0][col] == '.';
            for (size_t row = 0; row < map.size(); ++row)
            {
                if (map[row][col] != 'v')
                    continue;
                size_t next_row = row + 1;
                if (next_row >= map.size())
                {
                    if (!first_vacant)
                        continue;
                    next_row = 0;
                }
                if (map[next_row][col] != '.')
                    continue;
                map[row][col] = '.';
                map[next_row][col] = 'v';
                ++row;
                moved = true;
            }
        }

        return moved;
    }

    size_t RunUntilStall()
    {
        size_t count{};
        while (Step())
            ++count;
        return count + 1;
    }
};

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2021-25"_test = [] {
        Cucumber t1{std::istringstream{"...>>>>>..."}};
        expect(t1.Step());
        expect("...>>>>.>.."s == t1.Dump());
        expect(t1.Step());
        expect("...>>>.>.>."s == t1.Dump());

        Cucumber t2{std::istringstream{".>v....v..\n.......>.."}};
        expect(t2.Step());
        expect(".>........\n..v....v>."s == t2.Dump());

        Cucumber t3{std::istringstream{"...>...\n.......\n......>\nv.....>\n......>\n.......\n..vvv.."}};
        expect(t3.Step());
        expect("..vv>..\n.......\n>......\nv.....>\n>......\n.......\n....v.."s == t3.Dump());
        expect(t3.Step());
        expect("....v>.\n..vv...\n.>.....\n......>\nv>.....\n.......\n......."s == t3.Dump());
        expect(t3.Step());
        expect("......>\n..v.v..\n..>v...\n>......\n..>....\nv......\n......."s == t3.Dump());
        expect(t3.Step());
        expect(">......\n..v....\n..>.v..\n.>.v...\n...>...\n.......\nv......"s == t3.Dump());

        const char *const TEST = R"(
v...>>.vv>
.vv>>.vv..
>>.>v>...v
>>v>>.>.v.
v>v.vv.v..
>.>>..v...
.vv..>.>v.
v.v..>>v.v
....v..v.>
)";

        Cucumber t4{std::istringstream{TEST}};
        expect(t4.Step());
        const std::string TEST1 =
            "....>.>v.>\n"
            "v.v>.>v.v.\n"
            ">v>>..>v..\n"
            ">>v>v>.>.v\n"
            ".>v.v...v.\n"
            "v>>.>vvv..\n"
            "..v...>>..\n"
            "vv...>>vv.\n"
            ">.v.v..v.v";
        expect(eq(TEST1, t4.Dump()));
        expect(t4.Step());
        const std::string TEST2 =
            ">.v.v>>..v\n"
            "v.v.>>vv..\n"
            ">v>.>.>.v.\n"
            ">>v>v.>v>.\n"
            ".>..v....v\n"
            ".>v>>.v.v.\n"
            "v....v>v>.\n"
            ".vv..>>v..\n"
            "v>.....vv.";
        expect(eq(TEST2, t4.Dump()));

        Cucumber test{std::istringstream{TEST}};
        expect(58_u == test.RunUntilStall());

        Cucumber cucumber{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", cucumber.RunUntilStall());
    };
};

} //namespace;
