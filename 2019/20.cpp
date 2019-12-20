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
        std::unordered_map<std::string, int> halves;

        auto addFeature = [&](int r, int c, char a, char b) {
            auto idx = r * _width + c;

            std::string name;
            name.push_back(a);
            name.push_back(b);
            if (name == "AA")
                _start = idx;
            else if (name == "ZZ")
                _finish = idx;
            else
            {
                auto it = halves.find(name);
                if (it == halves.end())
                    halves[name] = idx;
                else
                {
                    _warps[it->second] = idx;
                    _warps[idx] = it->second;
                }
            }
        };

        int height = (_map.size() + _width - 1) / _width;
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
    }

    int FindWay() const
    {
        std::unordered_map<int, int> distance{{_start, 0}};
        std::queue<int> toProcess;
        toProcess.push(_start);

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
            auto cur = toProcess.front();
            toProcess.pop();

            auto curDist = distance.at(cur);
            if (cur == _finish)
                return curDist;

            probe(cur - 1, curDist + 1);
            probe(cur + 1, curDist + 1);
            probe(cur - _width, curDist + 1);
            probe(cur + _width, curDist + 1);

            auto warpIt = _warps.find(cur);
            if (warpIt != _warps.end())
                probe(warpIt->second, curDist + 1);
        }

        return -1;
    }

private:
    std::string _map;
    int _width;
    int _start;
    int _finish;
    std::unordered_map<int,int> _warps;

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
