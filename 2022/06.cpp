#include "../test.hpp"
#include <fstream>

namespace {

int FindStartOfPacket(const std::string &message)
{
    std::unordered_map<char, int> counts;

    for (int i = 0; i < message.size(); ++i)
    {
        ++counts[message[i]];
        if (i >= 4)
        {
            if (!(--counts[message[i - 4]]))
                counts.erase(message[i - 4]);
        }
        if (counts.size() == 4)
            return i + 1;
    }
    return -1;
}

using namespace boost::ut;

suite s = [] {
    "2022-06"_test = [] {
        expect(5_i == FindStartOfPacket("bvwbjplbgvbhsrlpgdmjqwftvncz"));
        expect(6_i == FindStartOfPacket("nppdvjthqldpwncqszvftbrmjlhg"));
        expect(10_i == FindStartOfPacket("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg"));
        expect(11_i == FindStartOfPacket("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw"));

        std::string input;
        std::getline(std::ifstream{INPUT}, input);
        Printer::Print(__FILE__, "1", FindStartOfPacket(input));
    };
};

} //namespace;
