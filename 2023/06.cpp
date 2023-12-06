#include "../test.hpp"
#include <ranges>

namespace {

using namespace boost::ut;

struct Course
{
    int time{};
    int distance{};
};

class Race
{
public:
    Race(Course course)
        : _course{course}
    {
    }

    int CalcWins() const
    {
        std::cout << UpperBound() << std::endl;
        std::cout << LowerBound() << std::endl;
        return UpperBound() - LowerBound();
    }

    static int Task1(int n, const Course *courses)
    {
        int res{1};
        for (int i = 0; i < n; ++i)
            res *= Race{courses[i]}.CalcWins();
        return res;
    }

private:
    Course _course;

    int CalcDistance(int time) const
    {
        return time * (_course.time - time);
    }

    int LowerBound() const
    {
        auto times = std::ranges::iota_view(0, _course.time / 2);
        auto it = std::upper_bound(times.begin(), times.end(), _course.distance,
                [this](int a, int b) { return a < CalcDistance(b); });
        return *it;
    }

    int UpperBound() const
    {
        auto times = std::ranges::iota_view(_course.time / 2, _course.time);
        auto it = std::lower_bound(times.begin(), times.end(), _course.distance,
                [this](int a, int b) { return CalcDistance(a) > b; });
        return *it;
    }
};

suite s = [] {
    "2023-06"_test = [] {
        Course test_courses[] = {{7, 9}, {15, 40}, {30, 200}};
        expect(4_i == Race{test_courses[0]}.CalcWins());
        expect(8_i == Race{test_courses[1]}.CalcWins());
        expect(9_i == Race{test_courses[2]}.CalcWins());
        expect(288_i == Race::Task1(std::size(test_courses), test_courses));

        Course courses[] = {{42, 284}, {68, 1005}, {69, 1122}, {85, 1341}};
        Printer::Print(__FILE__, "1", Race::Task1(std::size(courses), courses));
        //Printer::Print(__FILE__, "2", almanac.Task2());
    };
};

} //namespace;
