#include "../test.hpp"
#include <limits>
#include <fstream>
#include <list>
#include <stack>
#include <functional>
#include <cassert>

namespace {

class Number
{
public:
    static constexpr int LB = std::numeric_limits<int>::min();
    static constexpr int RB = std::numeric_limits<int>::min() + 1;
    using NumberT = std::list<int>;
    NumberT num;

    Number(const std::string &s)
    {
        auto num_started = false;
        for (auto c : s)
        {
            switch (c)
            {
            case '[':
                num.push_back(LB);
                num_started = false;
                break;
            case ']':
                num.push_back(RB);
                num_started = false;
                break;
            case '0'...'9':
                if (!num_started)
                {
                    num.push_back(c - '0');
                    num_started = true;
                }
                else
                {
                    num.back() = num.back() * 10 + (c - '0');
                }
                break;
            case ',':
                num_started = false;
                break;
            default:
                throw std::runtime_error("Invalid char");
            }
        }
    }

    Number(const Number &o) = default;

    std::string Dump() const
    {
        return Dump(nullptr, num.end());
    }

    std::string Dump(const char *msg, NumberT::const_iterator it) const
    {
        std::string res;
        if (msg)
        {
            res += msg;
            res += ": ";
        }
        std::stack<int> commas;
        commas.emplace();

        auto checkComma = [&commas, &res]() {
            if (commas.top())
                res += ',';
            ++commas.top();
        };

        for (auto i = num.begin(); i != num.end(); ++i)
        {
            switch (*i)
            {
            case LB:
                checkComma();
                if (i == it)
                    res += '*';
                res += '[';
                commas.emplace();
                break;
            case RB:
                if (i == it)
                    res += '*';
                res += ']';
                commas.pop();
                ++commas.top();
                break;
            default:
                checkComma();
                if (i == it)
                    res += '*';
                res += std::to_string(*i);
                break;
            }
        }
        return res;
    }

    Number& Reduce()
    {
        auto trackForward = [this](NumberT::iterator &it, int &level, std::function<bool(int&)> pred) {
            for (; it != num.end(); ++it)
            {
                switch (auto &n = *it)
                {
                case LB:
                    ++level;
                    break;
                case RB:
                    --level;
                    break;
                default:
                    if (pred(n))
                        return true;
                }
            }
            return false;
        };

        auto trackBackward = [this](NumberT::iterator &it, int &level, std::function<bool(int&)> pred) {
            for (; it != num.begin(); --it)
            {
                switch (auto &n = *it)
                {
                case LB:
                    --level;
                    break;
                case RB:
                    ++level;
                    break;
                default:
                    if (pred(n))
                        return true;
                }
            }
            return false;
        };

        //std::cout << Dump() << std::endl;

        // NOTE: It's possible to carefully track the iterators and levels
        // between last explode/split to avoid rescans from begin.
        while (true)
        {
            auto it = num.begin();
            int level = 0;
            while (true)
            {
                bool exploded = trackForward(it, level, [=,this,&it,&level](int &n) {
                    if (level <= 4)
                        return false;
                    //std::cout << Dump("Explode", it) << std::endl;
                    // explode
                    auto lnum = *it;
                    it = num.erase(it);
                    auto rnum = *it;
                    it = num.erase(it);
                    assert(*it == RB);
                    it = num.erase(it);
                    --it;
                    *it = 0;
                    --level;

                    auto itr{it}, itl{it};
                    auto lr{level}, ll{level};
                    if (trackForward(++itr, lr, [rnum](int &n) {
                            n += rnum;
                            return true;
                        }))
                    {
                        it = itr;
                        level = lr;
                    }

                    if (trackBackward(--itl, ll, [lnum](int &n) {
                            n += lnum;
                            return true;
                        }))
                    {
                        it = itl;
                        level = ll;
                    }

                    //std::cout << Dump("->", it) << std::endl;
                    return true;
                });
                if (!exploded)
                    break;
            }

            it = num.begin();
            level = 0;
            bool split = trackForward(it, level, [=,this,&it](int &n) {
                if (n <= 9)
                    return false;
                // split
                //std::cout << Dump("Split", it) << std::endl;
                int lnum = *it / 2;
                int rnum = *it - lnum;
                *it = RB;
                it = num.insert(it, rnum);
                it = num.insert(it, lnum);
                it = num.insert(it, LB);
                //std::cout << Dump("->", it) << std::endl;
                return true;
            });
            if (!split)
                break;
        }
        return *this;
    }

    Number& Add(Number &&o)
    {
        num.insert(num.begin(), LB);
        num.splice(num.end(), std::move(o.num));
        num.push_back(RB);
        return Reduce();
    }

    Number& Add(const Number &o)
    {
        num.insert(num.begin(), LB);
        num.insert(num.end(), o.num.begin(), o.num.end());
        num.push_back(RB);
        return Reduce();
    }

    using NumbersT = std::vector<Number>;

    static NumbersT ParseNumbers(std::istream &&is)
    {
        NumbersT numbers;
        std::string line;
        while (is >> line)
            numbers.emplace_back(Number{line});
        return numbers;
    }

    static Number ChainSum(const NumbersT &numbers)
    {
        auto it = numbers.begin();
        Number a{*it++};

        for (; it != numbers.end(); ++it)
            a.Add(*it);

        return a;
    }

    int CalcMagnitude() const
    {
        std::stack<int> accum;
        for (auto i : num)
        {
            switch (i)
            {
            case LB:
                break;
            case RB:
                {
                    auto a = accum.top();
                    a *= 2;
                    accum.pop();
                    accum.top() = accum.top() * 3 + a;
                    break;
                }
            default:
                accum.push(i);
                break;
            }
        }
        return accum.top();
    }

    static int FindMaxMagnitude(const NumbersT &numbers)
    {
        int max = 0;
        for (size_t i = 0; i < numbers.size(); ++i)
        {
            for (size_t j = 0; j < numbers.size(); ++j)
            {
                if (i == j)
                    continue;
                Number a = numbers[i];
                Number b = a.Add(numbers[j]);
                auto mag = b.CalcMagnitude();
                if (mag > max)
                    max = mag;
            }
        }
        return max;
    }
};

const char *TEST_INPUT = R"(
[[[0,[5,8]],[[1,7],[9,6]]],[[4,[1,2]],[[1,4],2]]]
[[[5,[2,8]],4],[5,[[9,9],0]]]
[6,[[[6,2],[5,6]],[[7,6],[4,7]]]]
[[[6,[0,7]],[0,9]],[4,[9,[9,0]]]]
[[[7,[6,4]],[3,[1,3]]],[[[5,5],1],9]]
[[6,[[7,3],[3,2]]],[[[3,8],[5,7]],4]]
[[[[5,4],[7,7]],8],[[8,3],8]]
[[9,3],[[9,9],[6,[4,9]]]]
[[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]]
[[[[5,2],5],[8,[3,7]]],[[5,[7,5]],[4,4]]]
)";

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "18"_test = [] {
        expect("[[[[[9,8],1],2],3],4]"s == Number{"[[[[[9,8],1],2],3],4]"}.Dump());
        expect("[[[[0,9],2],3],4]"s == Number{"[[[[[9,8],1],2],3],4]"}.Reduce().Dump());
        expect("[7,[6,[5,[7,0]]]]"s == Number{"[7,[6,[5,[4,[3,2]]]]]"}.Reduce().Dump());
        expect("[[6,[5,[7,0]]],3]"s == Number{"[[6,[5,[4,[3,2]]]],1]"}.Reduce().Dump());
        expect("[5,5]"s == Number{"10"}.Reduce().Dump());
        expect("[[5,6],[6,6]]"s == Number{"[11,12]"}.Reduce().Dump());
        expect("[[3,[2,[8,0]]],[9,[5,[7,0]]]]"s == Number{"[[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]"}.Reduce().Dump());

        expect("[[[[0,7],4],[[7,8],[6,0]]],[8,1]]"s == Number{"[[[[4,3],4],4],[7,[[8,4],9]]]"}.Add(Number{"[1,1]"}).Dump());
        expect("[[[[1,1],[2,2]],[3,3]],[4,4]]"s == Number::ChainSum(Number::ParseNumbers(std::istringstream{"[1,1]\n[2,2]\n[3,3]\n[4,4]"})).Dump());
        expect("[[[[3,0],[5,3]],[4,4]],[5,5]]"s == Number::ChainSum(Number::ParseNumbers(std::istringstream{"[1,1]\n[2,2]\n[3,3]\n[4,4]\n[5,5]"})).Dump());
        expect("[[[[5,0],[7,4]],[5,5]],[6,6]]"s == Number::ChainSum(Number::ParseNumbers(std::istringstream{"[1,1]\n[2,2]\n[3,3]\n[4,4]\n[5,5]\n[6,6]"})).Dump());
        expect("[[[[4,0],[5,4]],[[7,7],[6,0]]],[[8,[7,7]],[[7,9],[5,0]]]]"s ==
                Number{"[[[0,[4,5]],[0,0]],[[[4,5],[2,6]],[9,5]]]"}.Add(Number{"[7,[[[3,7],[4,3]],[[6,3],[8,8]]]]"}).Dump());

        expect("[[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]"s
                == Number::ChainSum(Number::ParseNumbers(std::istringstream{R"(
[[[0,[4,5]],[0,0]],[[[4,5],[2,6]],[9,5]]]
[7,[[[3,7],[4,3]],[[6,3],[8,8]]]]
[[2,[[0,8],[3,4]]],[[[6,7],1],[7,[1,6]]]]
[[[[2,4],7],[6,[0,5]]],[[[6,8],[2,8]],[[2,1],[4,5]]]]
[7,[5,[[3,8],[1,4]]]]
[[2,[2,2]],[8,[8,1]]]
[2,9]
[1,[[[9,3],9],[[9,0],[0,7]]]]
[[[5,[7,4]],7],1]
[[[[4,2],2],6],[8,7]]
                )"})).Dump());
        expect(29_i == Number{"[9,1]"}.CalcMagnitude());
        expect(129_i == Number{"[[9,1],[1,9]]"}.CalcMagnitude());
        auto test = Number::ParseNumbers(std::istringstream{TEST_INPUT});
        expect(4140_i == Number::ChainSum(test).CalcMagnitude());
        expect(3993_i == Number::FindMaxMagnitude(test));

        auto numbers = Number::ParseNumbers(std::fstream{INPUT});
        Printer::Print(__FILE__, "1", Number::ChainSum(numbers).CalcMagnitude());
        Printer::Print(__FILE__, "2", Number::FindMaxMagnitude(numbers));
    };
};

} //namespace;
