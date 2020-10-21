#include <string>
#include <openssl/md5.h>
#include <boost/ut.hpp>

#include "../Printer.hpp"

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

using namespace boost::ut;

suite s = [] {
    "2015-04"_test = [] {
        expect(609043_u == Mine("abcdef", HasFiveZeros));
        expect(1048970_u == Mine("pqrstuv", HasFiveZeros));

        Printer::Print(__FILE__, "1", Mine("bgvyzdsv", HasFiveZeros));
        Printer::Print(__FILE__, "2", Mine("bgvyzdsv", HasSixZeros));
    };
};

} //namespace;