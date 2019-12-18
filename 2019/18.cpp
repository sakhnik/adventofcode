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
#include <boost/scope_exit.hpp>


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

template <typename PositionT>
struct KnownPath
{
    PositionT position;
    KeySetT keysLeft;

    bool operator==(const KnownPath &o) const
    {
        return size(position) == size(o.position)
            && std::equal(begin(position), end(position), begin(o.position))
            && keysLeft == o.keysLeft;
    }

    bool operator<(const KnownPath &o) const
    {
        if (position < o.position) return true;
        if (position > o.position) return false;
        return keysLeft.to_ulong() < o.keysLeft.to_ulong();
    }
};

template <typename PositionT>
struct KnownPathHash
{
    typedef KnownPath<PositionT> argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const noexcept
    {
        result_type seed{0};
        boost::hash_combine(seed, s.position);
        boost::hash_combine(seed, s.keysLeft.to_ulong());
        return seed;
    }
};

template <typename PositionT, typename KnownPathsT>
int SearchImpl(PositionT &position,
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

    int minDist{MAX_DIST};

    for (auto &pos : position)
    {
        auto keys = GetKeysBfs(pos, map, width, keysLeft.count());

        for (auto &key : keys)
        {
            auto distanceToKey = key.dist;
            if (distanceToKey >= minDist)
                continue;

            // Pick up the key
            map[pos] = '.';
            BOOST_SCOPE_EXIT_ALL(&) {
                map[pos] = '@';
            };

            // Move the player to the key on the map
            map[key.position] = '@';
            BOOST_SCOPE_EXIT_ALL(&) {
                map[key.position] = key.key;
            };

            // Unlock the door
            auto doorTile = 'A' + (key.key - 'a');
            auto doorPosition = map.find(doorTile);
            if (doorPosition != map.npos)
                map[doorPosition] = ' ';
            BOOST_SCOPE_EXIT_ALL(&) {
                if (doorPosition != map.npos)
                    map[doorPosition] = doorTile;
            };

            // Modify the position
            auto oldPos = pos;
            pos = key.position;
            BOOST_SCOPE_EXIT_ALL(&) {
                pos = oldPos;
            };

            // Mark the key has been taken
            keysLeft.reset(key.key - 'a');
            BOOST_SCOPE_EXIT_ALL(&) {
                keysLeft.set(key.key - 'a');
            };

            auto newDist = distanceToKey + SearchImpl(position, map, width, keysLeft, knownPaths);
            if (newDist < minDist)
                minDist = newDist;
        }
    }

    knownPaths[{position, keysLeft}] = minDist;
    return minDist;
}

template <int N>
int SearchMin(std::string &map)
{
    auto keysLeft = std::accumulate(begin(map), end(map), KeySetT{},
                                    [](auto &set, char t) {
                                        if (t >= 'a' && t <= 'z')
                                            set.set(t - 'a');
                                        return set;
                                    });
    auto width = map.find("\n") + 1;

    using PositionT = std::array<int,N>;
    PositionT position;
    int p{0};

    for (int i = 0; i < N; ++i, ++p)
    {
        p = map.find("@", p);
        REQUIRE(p != map.npos);
        position[i] = p;
    }

    std::unordered_map<KnownPath<PositionT>, int, KnownPathHash<PositionT>> knownPaths;
    return SearchImpl(position, map, width, keysLeft, knownPaths);
}

TEST_CASE(TEST_NAME)
{
    {
        std::string m =
            "#########\n"
            "#b.A.@.a#\n"
            "#########";
        REQUIRE(8 == SearchMin<1>(m));
    }

    {
        std::string m =
            "########################\n"
            "#f.D.E.e.C.b.A.@.a.B.c.#\n"
            "######################.#\n"
            "#d.....................#\n"
            "########################";
        REQUIRE(86 == SearchMin<1>(m));
    }

    {
        std::string m =
            "########################\n"
            "#...............b.C.D.f#\n"
            "#.######################\n"
            "#.....@.a.B.c.d.A.e.F.g#\n"
            "########################";
        REQUIRE(132 == SearchMin<1>(m));
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
        REQUIRE(136 == SearchMin<1>(m));
    }

    {
        std::string m =
            "########################\n"
            "#@..............ac.GI.b#\n"
            "###d#e#f################\n"
            "###A#B#C################\n"
            "###g#h#i################\n"
            "########################";
        REQUIRE(81 == SearchMin<1>(m));
    }

    {
        std::ifstream ifs{INPUT};
        std::string map(std::istreambuf_iterator<char>{ifs},
                        std::istreambuf_iterator<char>{});

        MESSAGE(SearchMin<1>(map));

        auto width = map.find("\n") + 1;
        auto pos = map.find("@");

        map[pos-1-width] = '@';
        map[pos-width] = '#';
        map[pos-width+1] = '@';
        map[pos-1] = '#';
        map[pos] = '#';
        map[pos+1] = '#';
        map[pos+width-1] = '@';
        map[pos+width] = '#';
        map[pos+width+1] = '@';

        MESSAGE(SearchMin<4>(map));
    }
}
