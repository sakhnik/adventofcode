#include "../test.hpp"
#include <ranges>

namespace {

using namespace boost::ut;

struct Course
{
    int time{};
    int64_t distance{};
};

class Race
{
public:
    Race(Course course)
        : _course{course}
    {
    }

    int64_t CalcWins() const
    {
        std::cout << UpperBound() << std::endl;
        std::cout << LowerBound() << std::endl;
        return UpperBound() - LowerBound();
    }

    static int64_t Task1(size_t n, const Course *courses)
    {
        int64_t res{1};
        for (size_t i = 0; i < n; ++i)
            res *= Race{courses[i]}.CalcWins();
        return res;
    }

private:
    Course _course;

    int64_t CalcDistance(int64_t time) const
    {
        return static_cast<int64_t>(time) * (_course.time - time);
    }

    int64_t LowerBound() const
    {
        auto times = std::views::iota(0, _course.time / 2);
        // Dangerous application of operator<, this code may be implementation-dependent.
        auto it = std::upper_bound(times.begin(), times.end(), -1,
                [this](int a, int b) { return _course.distance < CalcDistance(b); });
        return *it;
    }

    int64_t UpperBound() const
    {
        auto times = std::ranges::iota_view(_course.time / 2, _course.time);
        auto it = std::lower_bound(times.begin(), times.end(), -1,
                [this](int64_t a, int64_t b) { return CalcDistance(a) > _course.distance; });
        return *it;
    }
};

suite s = [] {
    "06"_test = [] {
        Course test_courses[] = {{7, 9}, {15, 40}, {30, 200}};
        expect(4_i == Race{test_courses[0]}.CalcWins());
        expect(8_i == Race{test_courses[1]}.CalcWins());
        expect(9_i == Race{test_courses[2]}.CalcWins());
        expect(288_i == Race::Task1(std::size(test_courses), test_courses));
        expect(71503_i == Race{{71530, 940200ll}}.CalcWins());

        Course courses[] = {{42, 284}, {68, 1005}, {69, 1122}, {85, 1341}};
        Printer::Print(__FILE__, "1", Race::Task1(std::size(courses), courses));
        Printer::Print(__FILE__, "2", Race{{42686985ll, 284100511221341ll}}.CalcWins());
    };
};

} //namespace;
