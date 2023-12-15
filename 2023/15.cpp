#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Hash
{
    int task1{};

    struct Lens
    {
        std::string label;
        int focus = 0;
    };
    std::array<std::vector<Lens>, 256> boxes;

    Hash(std::istream &&is)
    {
        std::string step;
        while (getline(is, step, ','))
        {
            if (step.ends_with("\n"))
                step.pop_back();
            task1 += Calc(step);
            ProcessStep(step);
        }
    }

    int Task1() const { return task1; }

    int Task2() const
    {
        int sum{};
        for (int i = 0; i < boxes.size(); ++i)
        {
            const auto &box = boxes[i];
            for (int j = 0; j < box.size(); ++j)
            {
                sum += (i + 1) * (j + 1) * box[j].focus;
            }
        }
        return sum;
    }

    void ProcessStep(const std::string_view step)
    {
        auto sign = step.find_first_of("-=");
        std::string_view label{step.substr(0, sign)};
        auto hash = Calc(label);
        auto &box = boxes[hash];
        if (step[sign] == '-')
        {
            auto it = std::find_if(box.begin(), box.end(), [=](const Lens &l) { return l.label == label; });
            if (it != box.end())
                box.erase(it);
        }
        else
        {
            Lens lens;
            lens.label = label;
            lens.focus = std::stoi(std::string{step.substr(sign + 1)});
            auto it = std::find_if(box.begin(), box.end(), [=](const Lens &l) { return l.label == label; });
            if (it != box.end())
                *it = lens;
            else
                box.push_back(lens);
        }
    }

    static int Calc(std::string_view s, int val = 0)
    {
        for (int ch : s)
            val = (val + ch) * 17;
        return val & 0xff;
    }
};

suite s = [] {
    "2023-15"_test = [] {
        expect(52_i == Hash::Calc("HASH"));
        const char *const TEST1 = R"(rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7)";
        Hash test1{std::istringstream{TEST1}};
        expect(1320_i == test1.Task1());
        expect(145_i == test1.Task2());

        Hash hash{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", hash.Task1());
        Printer::Print(__FILE__, "2", hash.Task2());
    };
};

} //namespace;
