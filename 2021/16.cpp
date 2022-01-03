#include "../test.hpp"
#include <functional>
#include <fstream>
#include <numeric>
#include <bitset>

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

    size_t SumVersions() const
    {
        size_t sum{};
        auto on_packet = [&](unsigned version) {
            sum += version;
        };
        ParsePacket(_msg, on_packet);
        return sum;
    }

    struct Result
    {
        size_t length;
        int64_t value;
    };

    int64_t Calculate() const
    {
        auto on_packet = [&](unsigned version) {};
        return ParsePacket(_msg, on_packet).value;
    }

    static Result ParsePacket(std::string_view msg, OnPacketT on_packet)
    {
        int version = BinToNum(msg.substr(0, 3));
        int type = BinToNum(msg.substr(3, 3));
        on_packet(version);

        if (type == 4)
            return ParseLiteral(msg);

        Result res{};
        std::vector<int64_t> vals;

        if (msg[6] == '0')
        {
            size_t bit_length = BinToNum(msg.substr(7, 15));
            for (size_t l = 0; l < bit_length; )
            {
                auto r = ParsePacket(msg.substr(22 + l), on_packet);
                l += r.length;
                vals.push_back(r.value);
            }
            res.length = 22 + bit_length;
        }
        else
        {
            size_t packet_count = BinToNum(msg.substr(7, 11));
            size_t length = 0;
            for (size_t i = 0; i < packet_count; ++i)
            {
                auto r = ParsePacket(msg.substr(18 + length), on_packet);
                length += r.length;
                vals.push_back(r.value);
            }
            res.length = 18 + length;
        }

        switch (type)
        {
        case 0:  // sum
            res.value = std::accumulate(vals.begin(), vals.end(), 0ll, [](auto a, auto b) { return a + b; });
            break;
        case 1:  // product
            res.value = std::accumulate(vals.begin(), vals.end(), 1ll, [](auto a, auto b) { return a * b; });
            break;
        case 2:  // minimum
            res.value = *std::min_element(vals.begin(), vals.end());
            break;
        case 3:  // maximum
            res.value = *std::max_element(vals.begin(), vals.end());
            break;
        case 5:  // greater than
            res.value = vals[0] > vals[1];
            break;
        case 6:  // less than
            res.value = vals[0] < vals[1];
            break;
        case 7:  // equal to
            res.value = vals[0] == vals[1];
            break;
        }

        return res;
    }

    static Result ParseLiteral(std::string_view msg)
    {
        Result res{6 - 5, 0};
        auto &i = res.length;
        do
        {
            i += 5;
            std::string_view digit = msg.substr(i + 1, 4);
            res.value <<= 4;
            res.value |= BinToNum(digit);
        }
        while (msg[i] == '1');
        i += 5;
        return res;
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
        expect(21_u == Bits::ParseLiteral(Bits::HexToBin("D2FE28")).length);
        expect(2021_i == Bits::ParseLiteral(Bits::HexToBin("D2FE28")).value);
        expect(16_u == Bits{"8A004A801A8002F478"}.SumVersions());
        expect(12_u == Bits{"620080001611562C8802118E34"}.SumVersions());
        expect(23_u == Bits{"C0015000016115A2E0802F182340"}.SumVersions());
        expect(31_u == Bits{"A0016C880162017C3686B18A3D4780"}.SumVersions());
        expect(3_i == Bits{"C200B40A82"}.Calculate());
        expect(54_i == Bits{"04005AC33890"}.Calculate());
        expect(7_i == Bits{"880086C3E88112"}.Calculate());
        expect(9_i == Bits{"CE00C43D881120"}.Calculate());
        expect(1_i == Bits{"D8005AC2A8F0"}.Calculate());
        expect(0_i == Bits{"F600BC2D8F"}.Calculate());
        expect(0_i == Bits{"9C005AC2F8F0"}.Calculate());
        expect(1_i == Bits{"9C0141080250320F1802104A08"}.Calculate());

        Bits bits{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", bits.SumVersions());
        Printer::Print(__FILE__, "2", bits.Calculate());
    };
};

} //namespace;
