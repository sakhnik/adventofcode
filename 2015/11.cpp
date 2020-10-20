#include <string>
#include <boost/ut.hpp>

namespace {

void Increment(std::string &pass)
{
    auto i = pass.find_last_not_of("z");
    if (i != pass.npos)
    {
        ++pass[i];
        ++i;
    }
    else
    {
        i = 0;
    }
    for ( ; i < pass.size(); ++i)
    {
        pass[i] = 'a';
    }
}

std::string Increment(std::string &&pass)
{
    Increment(pass);
    return std::move(pass);
}

bool HasStraight(const std::string &pass)
{
    for (size_t i = 2; i < pass.size(); ++i)
    {
        if (pass[i] == pass[i-1] + 1 && pass[i-1] == pass[i-2] + 1)
        {
            return true;
        }
    }

    return false;
}

bool HasBannedLetters(const std::string &pass)
{
    auto i = pass.find_first_of("iol");
    return i != pass.npos;
}

int CountPairs(const std::string &pass)
{
    int count{};
    for (size_t i = 1; i < pass.size(); ++i)
    {
        if (pass[i] == pass[i-1])
        {
            ++count;
            ++i;
        }
    }
    return count;
}

bool IsValid(const std::string &pass)
{
    return HasStraight(pass) && !HasBannedLetters(pass) && CountPairs(pass) >= 2;
}

void Next(std::string &pass)
{
    do
    {
        Increment(pass);
    }
    while (!IsValid(pass));
}

std::string Next(std::string &&pass)
{
    Next(pass);
    return std::move(pass);
}

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2015-11.test"_test = [] {
        expect(eq("xy"s, Increment("xx")));
        expect(eq("xz"s, Increment("xy")));
        expect(eq("ya"s, Increment("xz")));
        expect(eq("yb"s, Increment("ya")));

        expect(!IsValid("hijklmmn"));
        expect(!IsValid("abbceffg"));
        expect(!IsValid("abbcegjk"));

        expect(eq("abcdffaa"s, Next("abcdefgh")));
        expect(eq("ghjaabcc"s, Next("ghijklmn")));
    };

    "2015-11.task"_test = [] {
        std::string pass("vzbxkghb");
        Next(pass);
        std::cout << "2015-11.1: " << pass << std::endl;
        Next(pass);
        std::cout << "2015-11.2: " << pass << std::endl;
    };
};

} //namespace;