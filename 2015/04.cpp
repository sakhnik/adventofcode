#include <doctest/doctest.h>
#include <string>
#include <openssl/md5.h>

namespace {

bool HasFiveZeros(unsigned char *md5)
{
    return md5[0] == 0 && md5[1] == 0 && (md5[2] >> 4) == 0;
}

bool HasSixZeros(unsigned char *md5)
{
    return md5[0] == 0 && md5[1] == 0 && md5[2] == 0;
}

template <typename Pred>
uint64_t Mine(const std::string &key, Pred pred)
{
    for (uint64_t i = 0; ; ++i)
    {
        std::string buf = key + std::to_string(i);
        unsigned char md5[MD5_DIGEST_LENGTH];
        MD5(reinterpret_cast<const uint8_t *>(buf.data()), buf.size(), md5);
        if (pred(md5))
        {
            return i;
        }
    }
}

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        REQUIRE(609043 == Mine("abcdef", HasFiveZeros));
        REQUIRE(1048970 == Mine("pqrstuv", HasFiveZeros));
    }

    SUBCASE("task") {
        MESSAGE(Mine("bgvyzdsv", HasFiveZeros));
        MESSAGE(Mine("bgvyzdsv", HasSixZeros));
    }
}
