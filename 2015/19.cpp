#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <boost/ut.hpp>

#include "../Printer.hpp"

namespace {

class Chem
{
public:
    Chem(std::istream &&is)
    {
        std::string line;
        while (is && getline(is, line))
        {
            if (line.empty())
                break;
            _Trans tr;
            std::string skip;
            std::istringstream{line} >> tr.from >> skip >> tr.to;
            _trans.emplace_back(std::move(tr));
        }

        if (is)
            getline(is, _formula);
    }

    size_t Count() const
    {
        std::unordered_set<std::string> res;
        for (const auto &tr : _trans)
        {
            for (size_t pos = _formula.find(tr.from);
                 pos != _formula.npos;
                 pos = _formula.find(tr.from, pos))
            {
                auto r = _formula.substr(0, pos) + tr.to + _formula.substr(pos + tr.from.size());
                res.emplace(std::move(r));
                ++pos;
            }
        
        }

        return res.size();
    }

    size_t Count2()
    {
        size_t count{};
        std::string cur{_formula};
        while (cur != "e")
        {
            size_t prev_count{count};
            for (const auto &tr : _trans)
            {
                auto i = cur.find(tr.to);
                if (i != cur.npos)
                {
                    ++count;
                    cur = cur.substr(0, i) + tr.from + cur.substr(i + tr.to.size());
                }
            }

            if (prev_count == count)
            {
                count = 0;
                cur = _formula;
                std::random_shuffle(_trans.begin(), _trans.end());
            }
        }
        return count;
    }

private:
    struct _Trans
    {
        std::string from;
        std::string to;
    };
    std::vector<_Trans> _trans;
    std::string _formula;
};

using namespace boost::ut;

suite s = [] {
    "2015-19.test"_test = [] {
        const char *const TEST =
            "H => HO\n"
            "H => OH\n"
            "O => HH\n"
            "\n"
            "HOH\n";
        Chem c(std::istringstream{TEST});
        expect(4_u == c.Count());
    };

    "2015-19.task"_test = [] {
        Chem c(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", c.Count());
        Printer::Print(__FILE__, "2", c.Count2());
    };
};

} //namespace;