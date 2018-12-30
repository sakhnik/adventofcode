#include <doctest/doctest.h>
#include <sstream>
#include <fstream>
#include <unordered_set>

namespace {

class Map
{
public:
    Map(std::istream &&is)
    {
        _Pos cur{0, 0};
        _houses.insert(cur);

        char dir{};
        while (is && (is >> dir))
        {
            switch (dir)
            {
            case '<': --cur.x; break;
            case '^': --cur.y; break;
            case '>': ++cur.x; break;
            case 'v': ++cur.y; break;
            }
            _houses.insert(cur);
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

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        REQUIRE(2 == Map{std::istringstream{">"}}.GetCount());
        REQUIRE(4 == Map{std::istringstream{"^>v<"}}.GetCount());
        REQUIRE(2 == Map{std::istringstream{"^v^v^v^v^v"}}.GetCount());
    }

    SUBCASE("task") {
        MESSAGE(Map{std::ifstream{INPUT}}.GetCount());
    }
}
