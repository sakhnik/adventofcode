#include "../test.hpp"
#include <iterator>
#include <fstream>

namespace {

using NumbersT = std::vector<std::string>;

NumbersT Parse(std::istream &&is)
{
    std::istream_iterator<std::string> first{is}, last{};
    return NumbersT(first, last);
}

size_t CalcConsumption(const NumbersT &numbers)
{
    std::vector<size_t> counts(numbers[0].size());
    for (const auto &number : numbers)
    {
        for (size_t i = 0; i < number.size(); ++i)
            counts[i] += number[i] - '0';
    }
    size_t gamma{}, epsilon{};
    for (size_t i = 0; i < counts.size(); ++i)
    {
        gamma = (gamma << 1) + (counts[i] > numbers.size() / 2);
        epsilon = (epsilon << 1) + (counts[i] <= numbers.size() / 2);
    }
    return gamma * epsilon;
}

template <typename IterT, typename PredT>
IterT Filter(IterT first, IterT last, int i, PredT pred)
{
    size_t count = last - first;
    size_t ones = std::count_if(first, last, [i](const auto &n) { return n[i] == '1'; });
    size_t zeros = count - ones;
    return std::partition(first, last, [=](const auto &n) { return pred(n[i], zeros, ones); });
}

template <typename PredT>
size_t Calc(NumbersT &numbers, PredT pred)
{
    auto it = numbers.end();
    for (size_t i = 0; i < numbers[0].size(); ++i)
    {
        it = Filter(numbers.begin(), it, i, pred);
        if (it == numbers.begin() + 1)
        {
            size_t val{};
            for (size_t j = 0; j < numbers[0].size(); ++j)
                val = (val << 1) + (numbers[0][j] - '0');
            return val;
        }
    }
    throw std::runtime_error("Filtering failed");
}

size_t CalcOxygen(NumbersT &numbers)
{
    return Calc(numbers, [](char ch, size_t zeros, size_t ones) {
        if (zeros == ones)
            return ch == '1';
        return ch == (zeros > ones ? '0' : '1');
    });
}

size_t CalcCarbonDioxide(NumbersT &numbers)
{
    return Calc(numbers, [](char ch, size_t zeros, size_t ones) {
        if (zeros == ones)
            return ch == '0';
        return ch == (zeros > ones ? '1' : '0');
    });
}

using namespace boost::ut;

const char *const TEST_INPUT = R"(00100
11110
10110
10111
10101
01111
00111
11100
10000
11001
00010
01010)";

suite s = [] {
    "2021-03"_test = [] {
        auto TEST_NUMBERS = Parse(std::istringstream{TEST_INPUT});
        expect(198_u == CalcConsumption(TEST_NUMBERS));
        auto NUMBERS = Parse(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", CalcConsumption(NUMBERS));

        expect(23_u == CalcOxygen(TEST_NUMBERS));
        expect(10_u == CalcCarbonDioxide(TEST_NUMBERS));
        Printer::Print(__FILE__, "2", CalcOxygen(NUMBERS) * CalcCarbonDioxide(NUMBERS));
    };
};

} //namespace;
