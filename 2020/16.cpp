#include "../test.hpp"
#include <algorithm>

namespace {

using namespace boost::ut;

struct Range
{
    int a, b;
    int c, d;

    bool Contains(int x) const
    {
        return (a <= x && x <= b) || (c <= x && x <= d);
    }
};

class Tickets
{
public:
    Tickets(std::istream &&is)
    {
        std::string line;
        while (getline(is, line) && !line.empty())
        {
            Range r;
            char field[64];
            int n = sscanf(line.c_str(), "%[^:]: %d-%d or %d-%d",
                           field, &r.a, &r.b, &r.c, &r.d);
            expect(n == 5);
            _fields.push_back({field, r});
        }

        bool check_nearby = false;
        while (getline(is, line))
        {
            if (line.empty() || line.starts_with("your ticket:"))
                continue;

            if (line.starts_with("nearby tickets:"))
            {
                check_nearby = true;
                continue;
            }

            std::istringstream iss{line};
            _TicketT ticket;
            for (size_t i = 0; i < _fields.size(); ++i)
            {
                int v{};
                char comma{};
                iss >> v >> comma;
                ticket.push_back(v);
                auto it = std::find_if(_fields.begin(), _fields.end(),
                                       [v](const auto &f) {
                                            return f.range.Contains(v);
                                       });
                if (it == _fields.end())
                {
                    if (check_nearby)
                    {
                        _error += v;
                        ticket.clear();  // Mark the ticket incomplete to skip it later.
                    }
                }
            }

            if (ticket.size() == _fields.size())
                _tickets.push_back(std::move(ticket));
        }
    }

    int GetError() const { return _error; }

    uint64_t Determine() const
    {
        // Field index -> ticket field index
        std::vector<size_t> field_mapping(_fields.size());
        std::iota(field_mapping.begin(), field_mapping.end(), 0);
        std::vector<size_t> col_mapping(_fields.size());
        std::iota(col_mapping.begin(), col_mapping.end(), 0);

        auto isFieldOk = [this](const _Field &fi, size_t ti) {
            auto it = std::find_if(_tickets.begin(), _tickets.end(),
                                   [&fi, ti](const auto &t) {
                                        return !fi.range.Contains(t[ti]);
                                   });
            return it == _tickets.end();
        };

        for (size_t i = 0; i < _fields.size(); ++i)
        {
            size_t min_count = std::numeric_limits<size_t>::max();
            size_t field_idx{};
            std::vector<size_t> min_choice;

            // Find field column that fits to the least count of ticket columns
            for (size_t j = i; j < _fields.size(); ++j)  // field
            {
                const _Field &field = _fields[field_mapping[j]];
                std::vector<size_t> choice;
                size_t count{};
                for (size_t k = i; k < _fields.size(); ++k)
                {
                    if (isFieldOk(field, col_mapping[k]))
                    {
                        ++count;
                        choice.push_back(k);
                    }
                }

                if (count < min_count)
                {
                    min_count = count;
                    field_idx = j;
                    min_choice.swap(choice);
                }
            }

            expect(min_count == 1);
            std::swap(field_mapping[i], field_mapping[field_idx]);
            std::swap(col_mapping[i], col_mapping[min_choice.front()]);
        }

        uint64_t res = 1;
        for (size_t i = 0; i < _fields.size(); ++i)
        {
            const auto &field = _fields[field_mapping[i]];
            if (field.name.starts_with("departure"))
                res *= _tickets[0][col_mapping[i]];
        }
        return res;
    }

private:
    struct _Field
    {
        std::string name;
        Range range;
    };
    std::vector<_Field> _fields;
    int _error = 0;
    using _TicketT = std::vector<int>;
    std::vector<_TicketT> _tickets;
};

suite s = [] {
    "2020-16"_test = [] {
        {
            const char *const TEST =
                R"(class: 1-3 or 5-7
row: 6-11 or 33-44
seat: 13-40 or 45-50

your ticket:
7,1,14

nearby tickets:
7,3,47
40,4,50
55,2,20
38,6,12)";
            Tickets t{std::istringstream{TEST}};
            expect(71_i == t.GetError());
        }

        Tickets tickets{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", tickets.GetError());
        Printer::Print(__FILE__, "2", tickets.Determine());
    };
};

} //namespace;
