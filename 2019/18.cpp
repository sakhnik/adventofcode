#include <doctest/doctest.h>
#include <fstream>
#include <iterator>
#include <queue>
#include <iostream>
#include <algorithm>
#include <bitset>
#include <numeric>
#include <unordered_map>
#include <boost/functional/hash.hpp>


const int MAX_DIST = std::numeric_limits<int>::max();

struct Key
{
    char key;
    int position;
    int dist;
};

using KeySetT = std::bitset<26>;

std::vector<Key> GetKeysBfs(int position, const std::string &map, int width, int keysLeft)
{
    std::vector<Key> keys;
    int distances[map.size()];
    std::fill(distances, distances + map.size(), MAX_DIST);
    distances[position] = 0;
    std::queue<int> toSearch;
    toSearch.push(position);

    while (!toSearch.empty())
    {
        int cur = toSearch.front();
        toSearch.pop();
        auto curDist = distances[cur];

        auto probe = [&](int i) {
            switch (auto tile = map[i])
            {
            case '#':
            case 'A'...'Z':
                break;
            default:
                if (distances[i] < MAX_DIST)
                    return;
                distances[i] = curDist + 1;
                toSearch.push(i);
                if (tile >= 'a' && tile <= 'z')
                {
                    keys.push_back({tile, i, distances[i]});
                    --keysLeft;
                }
            }
        };

        const int d[] = {-1, 1, -width, width};
        for (int j = 0; j < 4; ++j)
        {
            probe(cur + d[j]);
            if (!keysLeft)
                return keys;
        }
    }
    return keys;
}

struct KnownPath
{
    int position;
    KeySetT keysLeft;

    bool operator==(const KnownPath &o) const
    {
        return position == o.position && keysLeft == o.keysLeft;
    }

    bool operator<(const KnownPath &o) const
    {
        if (position < o.position) return true;
        if (position > o.position) return false;
        return keysLeft.to_ulong() < o.keysLeft.to_ulong();
    }
};

struct KnownPathHash
{
    typedef KnownPath argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const noexcept
    {
        result_type seed{0};
        boost::hash_combine(seed, s.position);
        boost::hash_combine(seed, s.keysLeft.to_ulong());
        return seed;
    }
};

using KnownPathsT = std::unordered_map<KnownPath, int, KnownPathHash>;

int SearchImpl(int position,
               std::string &map, int width,
               KeySetT keysLeft, KnownPathsT &knownPaths)
{
    {
        // Lookup whether we already calculated this arrangement
        auto it = knownPaths.find({position, keysLeft});
        if (it != knownPaths.end())
            return it->second;
    }

    if (keysLeft.none())
        return 0;

    auto keys = GetKeysBfs(position, map, width, keysLeft.count());

    int minDist{MAX_DIST};

    for (auto &key : keys)
    {
        auto distanceToKey = key.dist;
        if (distanceToKey >= minDist)
            continue;

        // Pick up the key
        map[position] = '.';
        map[key.position] = '@';       // move the player to the key
        auto doorTile = 'A' + (key.key - 'a');
        auto doorPosition = map.find(doorTile);
        if (doorPosition != map.npos)
            map[doorPosition] = ' ';  // unlock the door
        keysLeft.reset(key.key - 'a');

        auto newDist = distanceToKey + SearchImpl(key.position, map, width, keysLeft, knownPaths);

        keysLeft.set(key.key - 'a');
        if (doorPosition != map.npos)
            map[doorPosition] = doorTile;
        map[key.position] = key.key;
        map[position] = '@';

        if (newDist < minDist)
            minDist = newDist;
    }

    knownPaths[{position, keysLeft}] = minDist;
    return minDist;
}

int SearchMin(std::string &map)
{
    auto keysLeft = std::accumulate(begin(map), end(map), KeySetT{},
                                    [](auto &set, char t) {
                                        if (t >= 'a' && t <= 'z')
                                            set.set(t - 'a');
                                        return set;
                                    });
    auto width = map.find("\n") + 1;
    auto position = map.find("@");
    KnownPathsT knownPaths;
    return SearchImpl(position, map, width, keysLeft, knownPaths);
}

TEST_CASE(TEST_NAME)
{
    {
        std::string m =
            "#########\n"
            "#b.A.@.a#\n"
            "#########";
        REQUIRE(8 == SearchMin(m));
    }

    {
        std::string m =
            "########################\n"
            "#f.D.E.e.C.b.A.@.a.B.c.#\n"
            "######################.#\n"
            "#d.....................#\n"
            "########################";
        REQUIRE(86 == SearchMin(m));
    }

    {
        std::string m =
            "########################\n"
            "#...............b.C.D.f#\n"
            "#.######################\n"
            "#.....@.a.B.c.d.A.e.F.g#\n"
            "########################";
        REQUIRE(132 == SearchMin(m));
    }

    {
        std::string m =
            "#################\n"
            "#i.G..c...e..H.p#\n"
            "########.########\n"
            "#j.A..b...f..D.o#\n"
            "########@########\n"
            "#k.E..a...g..B.n#\n"
            "########.########\n"
            "#l.F..d...h..C.m#\n"
            "#################";
        REQUIRE(136 == SearchMin(m));
    }

    {
        std::string m =
            "########################\n"
            "#@..............ac.GI.b#\n"
            "###d#e#f################\n"
            "###A#B#C################\n"
            "###g#h#i################\n"
            "########################";
        REQUIRE(81 == SearchMin(m));
    }

    {
        std::ifstream ifs{INPUT};
        std::string map(std::istreambuf_iterator<char>{ifs},
                        std::istreambuf_iterator<char>{});

        MESSAGE(SearchMin(map));
    }
}
