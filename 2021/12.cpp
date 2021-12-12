#include "../test.hpp"
#include <boost/functional/hash.hpp>

namespace {

class Map
{
public:
    Map(std::istream &&is)
    {
        std::string line;
        while (is >> line)
        {
            auto sep = line.find("-");
            auto a = line.substr(0, sep);
            auto b = line.substr(sep + 1);
            _vertices[a].push_back(b);
            _vertices[b].push_back(a);
        }

        _State s{_count};
        _TrackPaths("start", s);
    }

    size_t GetCount() { return _count; }

private:
    using _AdjacenciesT = std::vector<std::string>;
    std::unordered_map<std::string, _AdjacenciesT> _vertices;
    size_t _count{};

    struct _State
    {
        _State(size_t &count): count{count} {}

        std::unordered_map<std::string, std::vector<std::string>> way;
        size_t &count;

        bool TryGo(const std::string &vert, const std::string &to)
        {
            auto &w = way[vert];

            // There can only be one destination from a lower case room
            if (vert != "start" && vert != "end" && std::islower(vert[0]))
            {
                if (w.empty())
                {
                    w.push_back(to);
                    return true;
                }
                return false;
            }

            // Can go only to unique destinations to avoid loops
            auto it = std::find(w.begin(), w.end(), to);
            if (it == w.end())
            {
                w.push_back(to);
                return true;
            }

            return false;
        }

        void Return(const std::string &vert)
        {
            auto &w = way[vert];
            assert(!w.empty());
            w.pop_back();
        }
    };

    void _TrackPaths(const std::string &vert, _State &state)
    {
        if (vert == "end")
        {
            ++state.count;
            //_PrintPath(state);
            return;
        }

        auto it = _vertices.find(vert);
        if (it == _vertices.end())
            return;

        for (auto &to : it->second)
        {
            if (to == "start")
                continue;
            if (state.TryGo(vert, to))
            {
                _TrackPaths(to, state);
                state.Return(vert);
            }
        }
    }

    void _PrintPath(const _State &state)
    {
        std::unordered_map<std::string, int> idx;
        std::string vert = "start";
        std::cout << vert;
        while (vert != "end")
        {
            vert = state.way.at(vert)[idx[vert]++];
            std::cout << " " << vert;
        }
        std::cout << std::endl;
    }
};

const char *TEST_INPUT = R"(
start-A
start-b
A-c
A-b
b-d
A-end
b-end
)";

const char *TEST_INPUT2 = R"(
dc-end
HN-start
start-kj
dc-start
dc-HN
LN-dc
HN-end
kj-sa
kj-HN
kj-dc
)";

const char *TEST_INPUT3 = R"(
fs-end
he-DX
fs-he
start-DX
pj-DX
end-zg
zg-sl
zg-pj
pj-he
RW-he
fs-DX
pj-RW
zg-RW
start-pj
he-WI
zg-he
pj-fs
start-RW
)";

using namespace boost::ut;

suite s = [] {
    "2021-12"_test = [] {
        Map test{std::istringstream{TEST_INPUT}};
        expect(10_u == test.GetCount());
        Map test2{std::istringstream{TEST_INPUT2}};
        expect(19_u == test2.GetCount());
        Map test3{std::istringstream{TEST_INPUT3}};
        expect(226_u == test3.GetCount());

        Map map{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", map.GetCount());
    };
};

} //namespace;
