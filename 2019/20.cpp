#include <fstream>
#include <iterator>
#include <sstream>
#include <unordered_map>
#include <queue>
#include "../test.hpp"

namespace {

using namespace boost::ut;

class Maze
{
public:
    Maze(std::istream &is)
        : _map(std::istreambuf_iterator<char>{is}, std::istreambuf_iterator<char>{})
        , _width(_map.find("\n") + 1)
    {
        int height = (_map.size() + _width - 1) / _width;

        auto isOuter = [&](int r, int c) {
            return r < 4 || r > height - 4 || c < 4 || c > _width - 5;
        };

        auto addFeature = [&](int r, int c, char a, char b) {
            auto idx = r * _width + c;

            std::string name;
            name.push_back(a);
            name.push_back(b);

            _features[idx] = name;
            (isOuter(r, c) ?  _outerFeatures[name] : _innerFeatures[name]) = idx;
        };

        for (int r = 0; r < height; ++r)
        {
            for (int c = 0; c < _width; ++c)
            {
                auto a = _GetTile(r, c);
                if (a >= 'A' && a <= 'Z')
                {
                    if (c + 1 < _width)
                    {
                        auto b = _GetTile(r, c+1);
                        if (b >= 'A' && b <= 'Z')
                        {
                            if (c - 1 >= 0 && _GetTile(r, c-1) == '.')
                            {
                                // left
                                addFeature(r, c-1, a, b);
                            }
                            else if (c + 2 < _width && _GetTile(r, c+2) == '.')
                            {
                                // right
                                addFeature(r, c+2, a, b);
                            }
                        }
                    }
                    if (r + 1 < height)
                    {
                        auto b = _GetTile(r+1, c);
                        if (b >= 'A' && b <= 'Z')
                        {
                            if (r-1 >= 0 && _GetTile(r-1, c) == '.')
                            {
                                // above
                                addFeature(r-1, c, a, b);
                            }
                            else if (r+2 < height && _GetTile(r+2, c) == '.')
                            {
                                // below
                                addFeature(r+2, c, a, b);
                            }
                        }
                    }
                }
            }
        }

        expect(_features.size() == _innerFeatures.size() * 2 + 2);
        expect(_innerFeatures.size() + 2 == _outerFeatures.size());
    }

    int FindWayTask1()
    {
        return FindWayImpl([this](int level, int pos) { return _Task1Rule(level, pos); });
    }

    int FindWayTask2()
    {
        return FindWayImpl([this](int level, int pos) { return _Task2Rule(level, pos); });
    }

private:
    std::string _map;
    int _width;
    std::unordered_map<int, std::string> _features;
    std::unordered_map<std::string, int> _outerFeatures;
    std::unordered_map<std::string, int> _innerFeatures;

    char _GetTile(int row, int col) const
    {
        size_t idx = row * _width + col;
        if (idx > _map.size())
            return '#';
        return _map[idx];
    }

    std::tuple<bool, int, int> _Task1Rule(int level, int pos) const
    {
        auto featureIt = _features.find(pos);
        if (featureIt != _features.end())
        {
            auto outerIt = _outerFeatures.find(featureIt->second);
            if (outerIt != _outerFeatures.end() && outerIt->second != pos)
                return {true, level, outerIt->second};
            else
            {
                auto innerIt = _innerFeatures.find(featureIt->second);
                if (innerIt != _innerFeatures.end() && innerIt->second != pos)
                    return {true, level, innerIt->second};
            }
        }
        return {false, 0, 0};
    }

    std::tuple<bool, int, int> _Task2Rule(int level, int pos) const
    {
        auto featureIt = _features.find(pos);
        if (featureIt == _features.end())
            return {false, 0, 0};

        auto innerIt = _innerFeatures.find(featureIt->second);
        if (innerIt != _innerFeatures.end())
        {
            if (innerIt->second != pos)
                return {level > 0, level - 1, innerIt->second};
        }

        auto outerIt = _outerFeatures.find(featureIt->second);
        if (outerIt != _outerFeatures.end())
        {
            if (outerIt->second != pos)
                return {true, level + 1, outerIt->second};
        }
        return {false, 0, 0};
    }

    template <typename RuleT>
    int FindWayImpl(RuleT rule) const
    {
        auto start = _outerFeatures.at("AA");
        auto finish = _outerFeatures.at("ZZ");

        // level -> position on the level -> distance
        std::unordered_map<int, std::unordered_map<int, int>> distances;
        distances[0] = {{start, 0}};
        // {level, position}
        std::queue<std::pair<int, int>> toProcess;
        toProcess.push({0, start});

        auto probe = [&](int level, int pos, int dist) {
            auto tile = _map[pos];
            if (tile != '.')
                return;
            auto &distancesOnLevel = distances[level];
            auto it = distancesOnLevel.find(pos);
            if (it != distancesOnLevel.end())
                return;
            distancesOnLevel[pos] = dist;
            toProcess.push({level, pos});
        };

        while (!toProcess.empty())
        {
            auto [level, curPos] = toProcess.front();
            toProcess.pop();

            auto curDist = distances.at(level).at(curPos);
            if (level == 0 && curPos == finish)
                return curDist;

            probe(level, curPos - 1, curDist + 1);
            probe(level, curPos + 1, curDist + 1);
            probe(level, curPos - _width, curDist + 1);
            probe(level, curPos + _width, curDist + 1);

            auto [teleport, newLevel, newPos] = rule(level, curPos);
            if (teleport)
                probe(newLevel, newPos, curDist + 1);
        }

        return -1;
    }

};

suite s = [] {
    "2019-20"_test = [] {
        {
            std::istringstream iss{
                "         A           \n"
                "         A           \n"
                "  #######.#########  \n"
                "  #######.........#  \n"
                "  #######.#######.#  \n"
                "  #######.#######.#  \n"
                "  #######.#######.#  \n"
                "  #####  B    ###.#  \n"
                "BC...##  C    ###.#  \n"
                "  ##.##       ###.#  \n"
                "  ##...DE  F  ###.#  \n"
                "  #####    G  ###.#  \n"
                "  #########.#####.#  \n"
                "DE..#######...###.#  \n"
                "  #.#########.###.#  \n"
                "FG..#########.....#  \n"
                "  ###########.#####  \n"
                "             Z       \n"
                "             Z       "
            };

            Maze m{iss};
            expect(23_i == m.FindWayTask1());
            expect(26_i == m.FindWayTask2());
        }

        {
            std::istringstream iss{
                "             Z L X W       C                 \n"
                "             Z P Q B       K                 \n"
                "  ###########.#.#.#.#######.###############  \n"
                "  #...#.......#.#.......#.#.......#.#.#...#  \n"
                "  ###.#.#.#.#.#.#.#.###.#.#.#######.#.#.###  \n"
                "  #.#...#.#.#...#.#.#...#...#...#.#.......#  \n"
                "  #.###.#######.###.###.#.###.###.#.#######  \n"
                "  #...#.......#.#...#...#.............#...#  \n"
                "  #.#########.#######.#.#######.#######.###  \n"
                "  #...#.#    F       R I       Z    #.#.#.#  \n"
                "  #.###.#    D       E C       H    #.#.#.#  \n"
                "  #.#...#                           #...#.#  \n"
                "  #.###.#                           #.###.#  \n"
                "  #.#....OA                       WB..#.#..ZH\n"
                "  #.###.#                           #.#.#.#  \n"
                "CJ......#                           #.....#  \n"
                "  #######                           #######  \n"
                "  #.#....CK                         #......IC\n"
                "  #.###.#                           #.###.#  \n"
                "  #.....#                           #...#.#  \n"
                "  ###.###                           #.#.#.#  \n"
                "XF....#.#                         RF..#.#.#  \n"
                "  #####.#                           #######  \n"
                "  #......CJ                       NM..#...#  \n"
                "  ###.#.#                           #.###.#  \n"
                "RE....#.#                           #......RF\n"
                "  ###.###        X   X       L      #.#.#.#  \n"
                "  #.....#        F   Q       P      #.#.#.#  \n"
                "  ###.###########.###.#######.#########.###  \n"
                "  #.....#...#.....#.......#...#.....#.#...#  \n"
                "  #####.#.###.#######.#######.###.###.#.#.#  \n"
                "  #.......#.......#.#.#.#.#...#...#...#.#.#  \n"
                "  #####.###.#####.#.#.#.#.###.###.#.###.###  \n"
                "  #.......#.....#.#...#...............#...#  \n"
                "  #############.#.#.###.###################  \n"
                "               A O F   N                     \n"
                "               A A D   M                     "
            };
            Maze m{iss};
            expect(396_i == m.FindWayTask2());
        }

        {
            std::ifstream ifs{INPUT};
            Maze m{ifs};
            Printer::Print(__FILE__, "1", m.FindWayTask1());
            Printer::Print(__FILE__, "2", m.FindWayTask2());
        }
    };
};

} //namespace;
