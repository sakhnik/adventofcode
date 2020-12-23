#include "../test.hpp"
#include <numeric>

namespace {

class Cups
{
public:
    Cups(const std::string &labels)
        : _next(labels.size())
    {
        for (size_t i = 0, n = labels.size() - 1; i < n; ++i)
        {
            _next[labels[i] - '1'] = labels[i + 1] - '1';
        }
        _next[labels.back() - '1'] = labels[0] - '1';

        _cur = labels.front() - '1';
    }

    void Move()
    {
        ssize_t buf1 = _next[_cur];
        ssize_t buf2 = _next[buf1];
        ssize_t buf3 = _next[buf2];
        _next[_cur] = _next[buf3];

        int dest = _cur;
        do
        {
            if (--dest < 0)
                dest = _next.size() - 1;
        }
        while (dest == buf1 || dest == buf2 || dest == buf3);

        size_t last = _next[dest];
        _next[dest] = buf1;
        _next[buf3] = last;

        _cur = _next[_cur];
    }

    void Move(int count)
    {
        while (count-- > 0)
            Move();
    }

    std::vector<size_t> GetLabels(int count = 0) const
    {
        if (!count)
            count = _next.size() - 1;

        std::vector<size_t> ret;
        size_t p = 0;
        while (count-- > 0)
        {
            p = _next[p];
            ret.push_back(p);
        }
        return ret;
    }

    std::string GetStr() const
    {
        auto labels = GetLabels();
        std::ostringstream oss;
        for (auto l : labels)
            oss << l + 1;
        return oss.str();
    }

private:
    std::vector<size_t> _next;
    size_t _cur = 0;
};

using namespace boost::ut;

suite s = [] {
    "2020-23"_test = [] {
        {
            Cups c{"389125467"};
            c.Move(100);
            expect(eq(c.GetStr(), std::string{"67384529"}));
        }

        Cups c{"215694783"};
        c.Move(100);
        Printer::Print(__FILE__, "1", c.GetStr());
    };
};

} //namespace;
