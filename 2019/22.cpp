#include <fstream>
#include <numeric>
#include <vector>
#include <string_view>
#include <sstream>
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/integer.hpp>
#include "../test.hpp"

namespace {

using namespace boost::ut;

using DeckT = std::vector<int>;

DeckT Deal(int count, std::istream &is)
{
    DeckT deck(count), deck2(count);
    std::iota(begin(deck), end(deck), 0);

    std::string line;
    while (getline(is, line))
    {
        if (line == "deal into new stack")
        {
            std::reverse(begin(deck), end(deck));
        }
        else if (std::string_view{line.data(), 4} == "cut ")
        {
            auto num = std::stoi(line.substr(4));
            if (num < 0)
                num += count;
            expect(num >= 0);
            expect(num < count);
            std::rotate(begin(deck), begin(deck) + num, end(deck));
        }
        else
        {
            const std::string INCR = "deal with increment ";
            expect(std::string_view{line.data(), INCR.size()} == INCR);
            auto num = std::stoi(line.substr(INCR.size()));
            for (int i{0}, idx{0}; i < count; ++i, idx += num)
            {
                if (idx > count)
                    idx -= count;
                deck2[idx] = deck[i];
            }
            deck.swap(deck2);
        }
    }

    return deck;
}


using IntT = boost::multiprecision::mpz_int;

int64_t Deal2(IntT count, IntT repeat, int pos, std::istream &is)
{
    using MatT = std::array<IntT, 4>;

    auto mult = [count](MatT a, MatT b) {
        MatT c = {
            ((a[0]*b[0] + a[1]*b[2]) % count + count) % count,
            ((a[0]*b[1] + a[1]*b[3]) % count + count) % count,
            ((a[2]*b[0] + a[3]*b[2]) % count + count) % count,
            ((a[2]*b[1] + a[3]*b[3]) % count + count) % count
        };
        return c;
    };

    MatT m{1, 0, 0, 1};

    std::string line;
    while (getline(is, line))
    {
        if (line == "deal into new stack")
        {
            m = mult(m, MatT{-1, count-1, 0, 1});
        }
        else if (std::string_view{line.data(), 4} == "cut ")
        {
            auto num = std::stoi(line.substr(4));
            expect(num < count);
            m = mult(m, MatT{1, num, 0, 1});
        }
        else
        {
            const std::string INCR = "deal with increment ";
            expect(std::string_view{line.data(), INCR.size()} == INCR);
            auto num = std::stoi(line.substr(INCR.size()));
            m = mult(m, MatT{powm(IntT{num}, count-2, count), 0, 0, 1});
        }
    }

    auto pow = [mult](MatT m, IntT exp, auto pow) {
        if (!exp)
            return MatT{1, 0, 0, 1};
        MatT ret = pow(m, exp >> 1, pow);
        ret = mult(ret, ret);
        return (exp & 1) ? mult(m, ret) : ret;
    };

    m = pow(m, repeat, pow);
    return ((m[0] * pos + m[1]) % count).convert_to<int64_t>();
}

suite s = [] {
    "22"_test = [] {
        {
            std::istringstream iss{
                "deal with increment 7\n"
                "deal into new stack\n"
                "deal into new stack\n"};
            DeckT ref{0, 3, 6, 9, 2, 5, 8, 1, 4, 7};
            expect(eq(ref, Deal(10, iss)));
        }

        {
            std::istringstream iss{"cut 3\n"};
            DeckT ref{3, 4, 5, 6, 7, 8, 9, 0, 1, 2};
            expect(eq(ref, Deal(10, iss)));
        }

        {
            std::istringstream iss{
                "cut 6\n"
                "deal with increment 7\n"
                "deal into new stack\n"};
            DeckT ref{3, 0, 7, 4, 1, 8, 5, 2, 9, 6};
            expect(eq(ref, Deal(10, iss)));
        }

        {
            std::istringstream iss{
                "deal with increment 7\n"
                "deal with increment 9\n"
                "cut -2\n"};
            DeckT ref{6, 3, 0, 7, 4, 1, 8, 5, 2, 9};
            expect(eq(ref, Deal(10, iss)));
        }

        {
            std::istringstream iss{
                "deal into new stack\n"
                "cut -2\n"
                "deal with increment 7\n"
                "cut 8\n"
                "cut -4\n"
                "deal with increment 7\n"
                "cut 3\n"
                "deal with increment 9\n"
                "deal with increment 3\n"
                "cut -1\n"};
            DeckT ref{9, 2, 5, 8, 1, 4, 7, 0, 3, 6};
            expect(eq(ref, Deal(10, iss)));
        }

        {
            std::ifstream ifs{INPUT};
            auto deck = Deal(10007, ifs);
            auto it = std::find(begin(deck), end(deck), 2019);
            Printer::Print(__FILE__, "1", it - begin(deck));
        }

        {
            std::ifstream ifs{INPUT};
            Printer::Print(__FILE__, "2", Deal2(119315717514047LL, 101741582076661LL, 2020, ifs));
        }
    };
};

} //namespace;
