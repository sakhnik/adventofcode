#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Config
{
    int red{}, green{}, blue{};

    bool IsSubset(const Config &o) const
    {
        return red <= o.red && green <= o.green && blue <= o.blue;
    }

    void BulkUp(const Config &o)
    {
        red = std::max(red, o.red);
        green = std::max(green, o.green);
        blue = std::max(blue, o.blue);
    }

    uint64_t GetPower() const
    {
        return red * green * blue;
    }
};

struct Game
{
    int id{};
    std::vector<Config> sets;
};

using GamesT = std::vector<Game>;

GamesT Parse(std::istream &&is)
{
    GamesT games;

    std::string game;
    while (getline(is, game))
    {
        Game g;

        std::istringstream iss{game};
        std::string discard;
        iss >> discard >> g.id >> discard;
        std::string set;
        while (getline(iss, set, ';'))
        {
            Config config;
            std::istringstream iss2{set};
            std::string cubes;
            while (getline(iss2, cubes, ','))
            {
                std::istringstream iss3{cubes};
                std::string color;
                int count{};
                iss3 >> count >> color;
                if (color == "red")
                    config.red = count;
                else if (color == "green")
                    config.green = count;
                else if (color == "blue")
                    config.blue = count;
            }
            g.sets.push_back(config);
        }

        games.push_back(std::move(g));
    }
    return games;
}

int Task1(const GamesT &games)
{
    const Config target_config = {.red = 12, .green = 13, .blue = 14};
    int sum{};

    for (const auto &game : games)
    {
        bool is_valid = true;
        for (const auto &set : game.sets)
        {
            if (!set.IsSubset(target_config))
            {
                is_valid = false;
                break;
            }
        }
        if (is_valid)
            sum += game.id;
    }
    return sum;
}

uint64_t Task2(const GamesT &games)
{
    uint64_t sum{};

    for (const auto &game : games)
    {
        Config config;
        for (const auto &set : game.sets)
            config.BulkUp(set);
        sum += config.GetPower();
    }
    return sum;
}

suite s = [] {
    "02"_test = [] {
        const char *const TEST1 = R"(Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green
)";
        auto test_games = Parse(std::istringstream{TEST1});
        expect(8_i == Task1(test_games));
        expect(2286_u == Task2(test_games));

        auto games = Parse(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", Task1(games));
        Printer::Print(__FILE__, "2", Task2(games));
    };
};

} //namespace;
