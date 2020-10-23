#include <fstream>
#include <vector>
#include "../test.hpp"

namespace {

using namespace boost::ut;

suite s = [] {
    "2019-16"_test = [] {
        using MsgT = std::vector<int>;

        auto getInput = [](std::istream &is) {
            std::string line;
            getline(is, line);

            std::vector<int> ret;
            for (auto ch : line)
                ret.push_back(ch - '0');
            return ret;
        };

        const int pattern[] = {0, 1, 0, -1};
        auto getPat = [&](int period, int i) {
            return pattern[((i + 1) / (period + 1)) % std::size(pattern)];
        };

        {
            expect(getPat(0, 0) == 1_i);
            expect(getPat(0, 1) == 0_i);
            expect(getPat(0, 2) == -1_i);
            expect(getPat(0, 3) == 0_i);

            expect(getPat(1, 0) == 0_i);
            expect(getPat(1, 1) == 1_i);
            expect(getPat(1, 2) == 1_i);
            expect(getPat(1, 3) == 0_i);
            expect(getPat(1, 4) == 0_i);
            expect(getPat(1, 5) == -1_i);
            expect(getPat(1, 6) == -1_i);
            expect(getPat(1, 7) == 0_i);
            expect(getPat(1, 8) == 0_i);
        }

        auto phase = [getPat](MsgT &msg) {
            MsgT res;

            for (size_t i = 0; i < msg.size(); ++i)
            {
                int v{};
                for (size_t j = 0; j < msg.size(); ++j)
                {
                    v += msg[j] * getPat(i, j);
                }
                if (v < 0)
                    v = -v;
                res.push_back(v % 10);
            }

            msg.swap(res);
        };

        {
            MsgT s1{1, 2, 3, 4, 5, 6, 7, 8};
            phase(s1);
            MsgT c1{4, 8, 2, 2, 6, 1, 5, 8};
            expect(std::equal(s1.begin(), s1.end(), c1.begin()));
        }

        auto toString = [](MsgT &msg, int count) {
            std::string res;
            for (int i = 0; i < count; ++i)
                res.push_back(msg[i] + '0');
            return res;
        };

        {
            // Task 1
            std::ifstream ifs{INPUT};
            auto msg = getInput(ifs);

            for (int i = 0; i < 100; ++i)
            {
                phase(msg);
            }

            Printer::Print(__FILE__, "1", toString(msg, 8));
        }

        {
            // Task 2
            std::ifstream ifs{INPUT};
            auto msg = getInput(ifs);

            size_t offset{};
            for (int i = 0; i < 7; ++i)
                offset = offset * 10 + msg[i];

            MsgT bigMsg;
            for (size_t i{offset}, in{10000 * msg.size()}; i < in; ++i)
                bigMsg.push_back(msg[i % msg.size()]);

            for (int i{0}; i < 100; ++i)
            {
                for (ssize_t j = bigMsg.size() - 2; j >= 0; --j)
                    bigMsg[j] = (bigMsg[j] + bigMsg[j + 1]) % 10;
            }

            Printer::Print(__FILE__, "2", toString(bigMsg, 8));
        }
    };
};

} //namespace;