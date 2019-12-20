#include <doctest/doctest.h>
#include <fstream>
#include <iterator>
#include <sstream>
#include <unordered_map>
#include <queue>
#include <iostream>


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

        CHECK(_features.size() == _innerFeatures.size() * 2 + 2);
        CHECK(_innerFeatures.size() + 2 == _outerFeatures.size());
    }

    int FindWay() const
    {
        auto start = _outerFeatures.at("AA");
        auto finish = _outerFeatures.at("ZZ");

        std::unordered_map<int, int> distance{{start, 0}};
        std::queue<int> toProcess;
        toProcess.push(start);

        auto probe = [&](int pos, int dist) {
            auto tile = _map[pos];
            if (tile != '.')
                return;
            auto it = distance.find(pos);
            if (it != distance.end())
                return;
            distance[pos] = dist;
            toProcess.push(pos);
        };

        while (!toProcess.empty())
        {
            auto curPos = toProcess.front();
            toProcess.pop();

            auto curDist = distance.at(curPos);
            if (curPos == finish)
                return curDist;

            probe(curPos - 1, curDist + 1);
            probe(curPos + 1, curDist + 1);
            probe(curPos - _width, curDist + 1);
            probe(curPos + _width, curDist + 1);

            auto featureIt = _features.find(curPos);
            if (featureIt != _features.end())
            {
                auto outerIt = _outerFeatures.find(featureIt->second);
                if (outerIt != _outerFeatures.end() && outerIt->second != curPos)
                    probe(outerIt->second, curDist + 1);
                else
                {
                    auto innerIt = _innerFeatures.find(featureIt->second);
                    if (innerIt != _innerFeatures.end() && innerIt->second != curPos)
                        probe(innerIt->second, curDist + 1);
                }
            }
        }

        return -1;
    }

private:
    std::string _map;
    int _width;
    std::unordered_map<int, std::string> _features;
    std::unordered_map<std::string, int> _outerFeatures;
    std::unordered_map<std::string, int> _innerFeatures;

    char _GetTile(int row, int col) const
    {
        auto idx = row * _width + col;
        if (idx > _map.size())
            return '#';
        return _map[idx];
    }
};


TEST_CASE(TEST_NAME)
{
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
        REQUIRE(23 == m.FindWay());
    }

    {
        std::ifstream ifs{INPUT};
        Maze m{ifs};
        MESSAGE(m.FindWay());
    }

}
