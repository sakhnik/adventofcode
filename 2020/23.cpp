#include "../test.hpp"
#include <numeric>

namespace {

class Cups
{
public:
    Cups(const std::string &labels, size_t count = 0)
        : _next(count ? count : labels.size())
    {
        if (!count)
            count = labels.size();

        auto get_label = [&](size_t i) {
            return i < labels.size() ? labels[i] - '1' : i;
        };

        for (size_t i = 0; i < count - 1; ++i)
        {
            auto cur = get_label(i);
            auto next = get_label(i + 1);
            _next[cur] = next;
        }
        _next[get_label(_next.size() - 1)] = get_label(0);

        _cur = get_label(0);
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
            ret.push_back(p + 1);
        }
        return ret;
    }

    std::string GetStr() const
    {
        auto labels = GetLabels();
        std::ostringstream oss;
        for (auto l : labels)
            oss << l;
        return oss.str();
    }

private:
    std::vector<size_t> _next;
    size_t _cur = 0;
};

using namespace boost::ut;

suite s = [] {
    "23"_test = [] {
        {
            Cups c{"389125467"};
            c.Move(100);
            expect(eq(c.GetStr(), std::string{"67384529"}));
        }

        {
            Cups c{"215694783"};
            c.Move(100);
            Printer::Print(__FILE__, "1", c.GetStr());
        }

        {
            Cups c{"389125467", 1000000};
            c.Move(10000000);
            auto res = c.GetLabels(2);
            expect(934001_u == res[0]);
            expect(159792_u == res[1]);
        }

        {
            Cups c{"215694783", 1000000};
            c.Move(10000000);
            auto res = c.GetLabels(2);
            Printer::Print(__FILE__, "2", res[0] * res[1]);
        }
    };
};

} //namespace;
