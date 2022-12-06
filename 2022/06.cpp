#include "../test.hpp"
#include <fstream>

namespace {

int FindStartOfPacket(int length, const std::string &message)
{
    std::unordered_map<char, int> counts;

    for (int i = 0; i < message.size(); ++i)
    {
        ++counts[message[i]];
        if (i >= length)
        {
            if (!(--counts[message[i - length]]))
                counts.erase(message[i - length]);
        }
        if (counts.size() == length)
            return i + 1;
    }
    return -1;
}

using namespace boost::ut;

suite s = [] {
    "2022-06"_test = [] {
        expect(5_i == FindStartOfPacket(4, "bvwbjplbgvbhsrlpgdmjqwftvncz"));
        expect(6_i == FindStartOfPacket(4, "nppdvjthqldpwncqszvftbrmjlhg"));
        expect(10_i == FindStartOfPacket(4, "nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg"));
        expect(11_i == FindStartOfPacket(4, "zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw"));

        expect(19_i == FindStartOfPacket(14, "mjqjpqmgbljsphdztnvjfqwrcgsmlb"));
        expect(23_i == FindStartOfPacket(14, "bvwbjplbgvbhsrlpgdmjqwftvncz"));
        expect(23_i == FindStartOfPacket(14, "nppdvjthqldpwncqszvftbrmjlhg"));
        expect(29_i == FindStartOfPacket(14, "nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg"));
        expect(26_i == FindStartOfPacket(14, "zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw"));

        std::string input;
        std::getline(std::ifstream{INPUT}, input);
        Printer::Print(__FILE__, "1", FindStartOfPacket(4, input));
        Printer::Print(__FILE__, "2", FindStartOfPacket(14, input));
    };
};

} //namespace;
