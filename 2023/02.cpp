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
};

int Task1(std::istream &&is)
{
    const Config target_config = {.red = 12, .green = 13, .blue = 14};
    int sum{};

    std::string game;
    while (getline(is, game))
    {
        bool is_valid = true;

        std::istringstream iss{game};
        std::string discard;
        int game_id{};
        iss >> discard >> game_id >> discard;
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
            if (!config.IsSubset(target_config))
                is_valid = false;
        }

        if (is_valid)
            sum += game_id;
    }
    return sum;
}

suite s = [] {
    "2023-02"_test = [] {
        const char *const TEST1 = R"(Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green
)";
        expect(8_i == Task1(std::istringstream{TEST1}));

        Printer::Print(__FILE__, "1", Task1(std::ifstream{INPUT}));
        //Printer::Print(__FILE__, "2", Calibrate2(std::ifstream{INPUT}));
    };
};

} //namespace;
