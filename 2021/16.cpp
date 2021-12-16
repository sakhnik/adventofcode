#include "../test.hpp"

namespace {

class Bits
{
public:
    Bits(std::istream &&is)
    {
        std::string hex;
        is >> hex;
        _msg = HexToBin(hex);
    }

    Bits(std::string_view hex)
        : _msg{HexToBin(hex)}
    {
    }

    using OnPacketT = std::function<void(unsigned version)>;

    size_t SumVersions()
    {
        size_t sum{};
        auto on_packet = [&](unsigned version) {
            sum += version;
        };
        ParsePacket(_msg, on_packet);
        return sum;
    }

    static size_t ParsePacket(std::string_view msg, OnPacketT on_packet)
    {
        int version = BinToNum(msg.substr(0, 3));
        int type = BinToNum(msg.substr(3, 3));
        on_packet(version);

        if (type == 4)
            return ParseLiteral(msg);
        if (msg[6] == '0')
        {
            size_t bit_length = BinToNum(msg.substr(7, 15));
            for (size_t l = 0; l < bit_length; )
                l += ParsePacket(msg.substr(22 + l), on_packet);
            return 22 + bit_length;
        }
        else
        {
            size_t packet_count = BinToNum(msg.substr(7, 11));
            size_t length = 0;
            for (size_t i = 0; i < packet_count; ++i)
                length += ParsePacket(msg.substr(18 + length), on_packet);
            return 18 + length;
        }

    }

    static size_t ParseLiteral(std::string_view msg)
    {
        size_t i = 6 - 5;
        do
        {
            i += 5;
            //std::string_view digit = msg.substr(i + 1, 4);
        }
        while (msg[i] == '1');
        return i + 5;
    }

    static uint64_t BinToNum(std::string_view bin)
    {
        uint64_t v{};
        for (auto d : bin)
        {
            v <<= 1;
            v |= d - '0';
        }
        return v;
    }

    static std::string HexToBin(std::string_view hex)
    {
        std::string res;
        res.reserve(hex.size() * 4);
        for (auto d : hex)
            res += std::bitset<4>{std::stoul(std::string{d}, nullptr, 16)}.to_string();
        return res;
    }

private:
    std::string _msg;

};

const char *TEST_INPUT = R"()";

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2021-16"_test = [] {
        Bits test{std::istringstream{TEST_INPUT}};
        expect(21_u == Bits::ParseLiteral(Bits::HexToBin("D2FE28")));
        expect(16_u == Bits{"8A004A801A8002F478"}.SumVersions());
        expect(12_u == Bits{"620080001611562C8802118E34"}.SumVersions());
        expect(23_u == Bits{"C0015000016115A2E0802F182340"}.SumVersions());
        expect(31_u == Bits{"A0016C880162017C3686B18A3D4780"}.SumVersions());

        Bits bits{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", bits.SumVersions());
    };
};

} //namespace;
