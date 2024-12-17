#include "../test.hpp"

namespace {

using namespace boost::ut;

std::string Run(int64_t a, const std::string &program, bool comma = false)
{
    std::array<int64_t, 3> reg = {a, 0, 0};
    int pc{0};

    auto getCombo = [&](int o) -> int64_t {
        switch (o) {
        case 0 ... 3:
            return o;
        case 4 ... 6:
            return reg[o - 4];
        default:
            assert(false);
        }
    };

    std::string out;
    while (pc < program.size()) {
        int64_t v = program[pc + 1] - '0';
        switch (program[pc] - '0') {
        case 0:  // adv
            reg[0] /= (1 << getCombo(v));
            break;
        case 1:  // bxl
            reg[1] ^= v;
            break;
        case 2:  // bst
            reg[1] = 7 & getCombo(v);
            break;
        case 3:  // jnz
            if (reg[0]) {
                pc = v;
                continue;
            }
            break;
        case 4:  // bxc
            reg[1] ^= reg[2];
            break;
        case 5:  // out
            if (comma && !out.empty())
                out.push_back(',');
            out.push_back('0' + (7 & getCombo(v)));
            break;
        case 6:  // bdv
            reg[1] = reg[0] / (1 << getCombo(v));
            break;
        case 7:  // cdv
            reg[2] = reg[0] / (1 << getCombo(v));
            break;
        }
        pc += 2;
    }

    return out;
}

int Task(const std::string &program)
{
    for (int a = 0; ; ++a) {
        if (program == Run(a, program))
            return a;
    }
}

int Task2()
{
    std::vector<int> program = {2,4,1,5,7,5,1,6,0,3,4,6,5,5,3,0};

    for (int64_t A = 1ll << (16 * 3); ; ++A) {
        int64_t a = A;
        int64_t b = 0;
        int64_t c = 0;
        int i = 0;

        do {
            // 2,4
            b = 7 & a;
            // 1,5
            b ^= 5;
            // 7,5
            c = a / (1 << b);
            // 1,6
            b ^= 6;
            // 0,3
            a >>= 3;
            // 4,6
            b ^= c;
            // 5,5
            if (program[i++] != b)
                break;
            // 3,0
        } while (a);

        if (i == program.size())
            return A;
    }
}

int64_t Task3(const std::string &program)
{
    // Search by 3 bits from the end to the begin
    std::vector<int64_t> variantsA = {0};
    for (int i = 0; i < program.size(); ++i) {
        std::vector<int64_t> newA;
        for (auto prevA : variantsA) {
            for (int j = 0; j < 8; ++j) {
                int64_t a = (prevA << 3) | j;
                if (Run(a, program) == program.substr(program.size() - i - 1))
                    newA.push_back(a);
            }
        }
        variantsA.swap(newA);
    }
    return *std::min_element(variantsA.begin(), variantsA.end());
}

suite s = [] {
    "17"_test = [] {
        expect(eq(std::string{"4635635210"}, Run(729, "015430")));
        expect(117440_i == Task3("035430"));

        Printer::Print(__FILE__, "1", Run(51064159, "2415751603465530", true));
        Printer::Print(__FILE__, "2", Task3("2415751603465530"));

    };
};

} //namespace;
