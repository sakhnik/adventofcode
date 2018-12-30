#include <doctest/doctest.h>
#include <sstream>
#include <fstream>
#include <unordered_set>
#include <vector>

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

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test1") {
        REQUIRE(2 == Map(1, std::istringstream{">"}).GetCount());
        REQUIRE(4 == Map(1, std::istringstream{"^>v<"}).GetCount());
        REQUIRE(2 == Map(1, std::istringstream{"^v^v^v^v^v"}).GetCount());
    }

    SUBCASE("task1") {
        MESSAGE(Map(1, std::ifstream{INPUT}).GetCount());
    }

    SUBCASE("test2") {
        REQUIRE(3 == Map(2, std::istringstream{"^v"}).GetCount());
        REQUIRE(3 == Map(2, std::istringstream{"^>v<"}).GetCount());
        REQUIRE(11 == Map(2, std::istringstream{"^v^v^v^v^v"}).GetCount());
    }

    SUBCASE("task2") {
        MESSAGE(Map(2, std::ifstream{INPUT}).GetCount());
    }
}
