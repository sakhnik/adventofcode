#include "IntCode.h"
#include <fstream>
#include "../test.hpp"


namespace {

using namespace boost::ut;

suite s = [] {
    "09"_test = [] {
        {
            const char *t = "109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99";
            IntCodeB p{std::istringstream{t}};
            int res[] = {109, 1, 204, -1, 1001, 100, 1, 100, 1008, 100, 16, 101, 1006, 101, 0, 99};
            for (int r : res)
            {
                expect(eq(r, p.Advance(0)));
                expect(eq(p.GetState(), p.S_OUTPUT));
            }
            p.Advance(0);
            expect(eq(p.GetState(), p.S_HALT));
        }

        {
            IntCodeB p{std::istringstream{"1102,34915192,34915192,7,4,7,99,0"}};
            auto r = p.Advance(0);
            expect(eq(p.GetState(), p.S_OUTPUT));
            expect(16_u == r.str().size());
        }

        std::ifstream ifs{INPUT};
        IntCodeB program{ifs};

        {
            IntCodeB t{program};
            auto r = t.Advance(0);
            expect(eq(t.GetState(), t.S_INPUT));
            r = t.Advance(1);
            expect(eq(t.GetState(), t.S_OUTPUT));
            Printer::Print(__FILE__, "1", r.convert_to<std::string>());
        }

        {
            IntCodeB t{program};
            auto r = t.Advance(0);
            expect(eq(t.GetState(), t.S_INPUT));
            r = t.Advance(2);
            expect(eq(t.GetState(), t.S_OUTPUT));
            Printer::Print(__FILE__, "2", r.convert_to<std::string>());
        }
    };
};

} //namespace;
