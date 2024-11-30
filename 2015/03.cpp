#include <sstream>
#include <fstream>
#include <unordered_set>
#include <vector>
#include "../test.hpp"
namespace {

class Map
{
public:
    Map(int count, std::istream &&is)
    {
        std::vector<_Pos> clones(count, {0, 0});
        _houses.insert(clones[0]);
        int i = 0;

        char dir{};
        while (is && (is >> dir))
        {
            auto &cur = clones[i];
            switch (dir)
            {
            case '<': --cur.x; break;
            case '^': --cur.y; break;
            case '>': ++cur.x; break;
            case 'v': ++cur.y; break;
            }
            _houses.insert(cur);
            i = (i + 1) % clones.size();
        }
    }

    size_t GetCount() const
    {
        return _houses.size();
    }

private:
    struct _Pos
    {
        int x, y;
        bool operator==(const _Pos &o) const { return x == o.x && y == o.y; }
    };
    static constexpr auto _posHash = [](const _Pos &p) {
        return std::hash<uint64_t>()((uint64_t(p.x) << 32) | p.y);
    };

    std::unordered_set<_Pos, decltype(_posHash)> _houses{0, _posHash};
};

using namespace boost::ut;

suite s = [] {
    "03"_test = [] {
        expect(2_u == Map(1, std::istringstream{">"}).GetCount());
        expect(4_u == Map(1, std::istringstream{"^>v<"}).GetCount());
        expect(2_u == Map(1, std::istringstream{"^v^v^v^v^v"}).GetCount());

        Printer::Print(__FILE__, "1", Map(1, std::ifstream{INPUT}).GetCount());

        expect(3_u == Map(2, std::istringstream{"^v"}).GetCount());
        expect(3_u == Map(2, std::istringstream{"^>v<"}).GetCount());
        expect(11_u == Map(2, std::istringstream{"^v^v^v^v^v"}).GetCount());

        Printer::Print(__FILE__, "2", Map(2, std::ifstream{INPUT}).GetCount());
    };
};

} //namespace;
