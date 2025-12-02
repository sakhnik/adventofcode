#include <iostream>
#include <sstream>
#include <cstdint>

struct Task
{
    uint64_t count1 = 0;
    uint64_t count2 = 0;

    Task(std::istream &&is)
    {
        std::string range;
        while (std::getline(is, range, ',')) {
            auto sep = range.find('-');
            uint64_t start = std::stoull(range.substr(0, sep));
            uint64_t finish = std::stoull(range.substr(sep + 1));
            for (auto a = start; a <= finish; ++a) {
                if (IsInvalid(std::to_string(a)))
                    count1 += a;
                if (IsInvalid2(std::to_string(a))) {
                    count2 += a;
                }
            }
        }
    }

    bool IsInvalid(std::string &&s)
    {
        return s.size() % 2 == 0 && s.substr(0, s.size() / 2) == s.substr(s.size() / 2);
    }

    bool IsInvalid2(std::string &&s)
    {
        std::string doubled = s + s;
        auto pos = doubled.find(s, 1);
        return pos != doubled.npos && pos < s.size();
    }
};

int main()
{
    const char *const TEST = "11-22,95-115,998-1012,1188511880-1188511890,222220-222224,1698522-1698528,446443-446449,38593856-38593862,565653-565659,824824821-824824827,2121212118-2121212124";
    Task test{std::istringstream{TEST}};

    const char *const IN = "82853534-82916516,2551046-2603239,805115-902166,3643-7668,4444323719-4444553231,704059-804093,32055-104187,7767164-7799624,25-61,636-1297,419403897-419438690,66-143,152-241,965984-1044801,1-19,376884-573880,9440956-9477161,607805-671086,255-572,3526071225-3526194326,39361322-39455443,63281363-63350881,187662-239652,240754-342269,9371-26138,1720-2729,922545-957329,3477773-3688087,104549-119841";
    Task task{std::istringstream{IN}};
    std::cout << task.count1 << std::endl;
    std::cout << task.count2 << std::endl;
    return 0;
}
