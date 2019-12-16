#include <doctest/doctest.h>
#include <fstream>
#include <vector>
#include <iostream>


TEST_CASE(TEST_NAME)
{
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
        REQUIRE(getPat(0, 0) == 1);
        REQUIRE(getPat(0, 1) == 0);
        REQUIRE(getPat(0, 2) == -1);
        REQUIRE(getPat(0, 3) == 0);

        REQUIRE(getPat(1, 0) == 0);
        REQUIRE(getPat(1, 1) == 1);
        REQUIRE(getPat(1, 2) == 1);
        REQUIRE(getPat(1, 3) == 0);
        REQUIRE(getPat(1, 4) == 0);
        REQUIRE(getPat(1, 5) == -1);
        REQUIRE(getPat(1, 6) == -1);
        REQUIRE(getPat(1, 7) == 0);
        REQUIRE(getPat(1, 8) == 0);
    }

    auto phase = [getPat](MsgT &msg) {
        MsgT res;

        for (int i = 0; i < msg.size(); ++i)
        {
            int v{};
            for (int j = 0; j < msg.size(); ++j)
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
        MsgT s1{1,2,3,4,5,6,7,8};
        phase(s1);
        MsgT c1{4,8,2,2,6,1,5,8};
        REQUIRE(std::equal(s1.begin(), s1.end(), c1.begin()));
    }

    auto toString = [](MsgT &msg) {
        std::string res;
        for (auto v : msg)
            res.push_back(v + '0');
        return res;
    };

    {
        std::ifstream ifs{INPUT};
        auto msg = getInput(ifs);

        for (int i = 0; i < 100; ++i)
        {
            phase(msg);
        }

        MESSAGE(toString(msg).substr(0, 8));
    }
}
