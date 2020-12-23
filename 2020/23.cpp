#include "../test.hpp"

namespace {

class Cups
{
public:
    Cups(const std::string &labels)
        : _labels(labels.begin(), labels.end())
    {
        _min = *std::min_element(_labels.begin(), _labels.end());
        _max = *std::max_element(_labels.begin(), _labels.end());
    }

    void Move()
    {
        std::rotate(_labels.begin(), _labels.begin() + _cur, _labels.end());

        std::array<char, 3> buffer;
        auto it = _labels.begin() + 1;
        std::copy(it, it + 3, buffer.begin());
        _labels.erase(it, it + 3);

        auto cup = _labels.front(), dest = cup;
        do
        {
            if (--dest < _min)
                dest = _max;
        }
        while (std::find(buffer.begin(), buffer.end(), dest) != buffer.end());

        it = std::find(_labels.begin(), _labels.end(), dest) + 1;
        _labels.insert(it, buffer.begin(), buffer.end());

        std::rotate(_labels.begin(), _labels.begin() + _labels.size() - _cur, _labels.end());
        if (++_cur >= _labels.size())
            _cur = 0;
    }

    void Move(int count)
    {
        while (count-- > 0)
            Move();
    }

    void PopCup1()
    {
        auto it = std::find(_labels.begin(), _labels.end(), '1');
        auto d = it - _labels.begin();

        std::rotate(_labels.begin(), it, _labels.end());
        _cur += _labels.size() - d;
        _cur %= _labels.size();
    }

    std::string GetLabels(size_t skip = 0) const
    {
        return {_labels.begin() + skip, _labels.end()};
    }

private:
    std::vector<char> _labels;
    size_t _cur = 0;
    char _min;
    char _max;
};

using namespace boost::ut;

suite s = [] {
    "2020-23"_test = [] {
        {
            Cups c{"389125467"};
            c.Move(100);
            c.PopCup1();
            expect(eq(c.GetLabels(1), std::string{"67384529"}));
        }

        Cups c{"215694783"};
        c.Move(100);
        c.PopCup1();
        Printer::Print(__FILE__, "1", c.GetLabels(1));
    };
};

} //namespace;
