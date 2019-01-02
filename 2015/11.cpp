#include <doctest/doctest.h>
#include <string>

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

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        REQUIRE("xy" == Increment("xx"));
        REQUIRE("xz" == Increment("xy"));
        REQUIRE("ya" == Increment("xz"));
        REQUIRE("yb" == Increment("ya"));

        REQUIRE(!IsValid("hijklmmn"));
        REQUIRE(!IsValid("abbceffg"));
        REQUIRE(!IsValid("abbcegjk"));

        REQUIRE("abcdffaa" == Next("abcdefgh"));
        REQUIRE("ghjaabcc" == Next("ghijklmn"));
    }

    SUBCASE("task") {
        MESSAGE(Next("vzbxkghb"));
    }
}
