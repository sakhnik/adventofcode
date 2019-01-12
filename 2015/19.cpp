#include <doctest/doctest.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <unordered_set>

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

private:
    struct _Trans
    {
        std::string from;
        std::string to;
    };
    std::vector<_Trans> _trans;
    std::string _formula;
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        const char *const TEST =
            "H => HO\n"
            "H => OH\n"
            "O => HH\n"
            "\n"
            "HOH\n";
        Chem c(std::istringstream{TEST});
        REQUIRE(4 == c.Count());
    }

    SUBCASE("task") {
        Chem c(std::ifstream{INPUT});
        MESSAGE(c.Count());
    }
}
