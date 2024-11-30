#include "Cpu.h"
#include <sstream>
#include <fstream>
#include "../test.hpp"

namespace {

using namespace boost::ut;

suite s = [] {
    "19"_test = [] {
        using namespace aoc2018;

        {
            Cpu cpu(std::istringstream{
                "#ip 0\n"
                "seti 5 0 1\n"
                "seti 6 0 2\n"
                "addi 0 1 0\n"
                "addr 1 2 3\n"
                "setr 1 0 0\n"
                "seti 8 0 4\n"
                "seti 9 0 5\n"});
            expect(Cpu::RegsT{0, 0, 0, 0, 0, 0} == cpu.GetRegs());
            cpu.Step();
            expect(Cpu::RegsT{1, 5, 0, 0, 0, 0} == cpu.GetRegs());
            cpu.Step();
            expect(Cpu::RegsT{2, 5, 6, 0, 0, 0} == cpu.GetRegs());
            cpu.Step();
            expect(Cpu::RegsT{4, 5, 6, 0, 0, 0} == cpu.GetRegs());
            cpu.Step();
            expect(Cpu::RegsT{6, 5, 6, 0, 0, 0} == cpu.GetRegs());
        }

        {
            Cpu cpu(std::ifstream{INPUT});
            cpu.Execute();
            Printer::Print(__FILE__, "1", cpu.GetRegs()[0]);
        }

        {
            //Cpu cpu(std::ifstream{INPUT}, {1, 0, 0, 0, 0, 0});
            //cpu.Execute();
            //MESSAGE(cpu.GetRegs()[0]);
            //cpu.Print();

            //           +- 0. jump 17             // r2 = r2 + 16
            //         +-|> 1. r1 = 1
            //     +---|-|> 2. r3 = 1
            //     | +-|-|> 3. r5 = r1 * r3
            //     | | | |  4. r5 = r5 == r4
            //     | | | |  5. jump +[r5 + 1]      // r2 = r5 + r2
            //     | | | |  6. jump +2             // r2 = r2 + 1
            //     | | | |  7. r0 += r1            // r0 = r1 + r0
            //     | | | |  8. ++r3                // r3 = r3 + 1
            //     | | | |  9. r5 = r3 > r4
            //     | | | |  10. jump +[r5 + 1]     // r2 = r2 + r5
            //     | +-|-|- 11. jump 3             // r2 = 2
            //     |   | |  12. ++r1               // r1 = r1 + 1
            //     |   | |  13. r5 = r1 > r4
            //     |   | |  14. jump +[r5 + 1]     // r2 = r5 + r2
            //     +---|-|- 15. jump 2             // r2 = 1
            //         | |  16. halt               // r2 = r2 * r2
            //         | +> 17. r4 += 2            // r4 = r4 + 2
            //         |    18. r4 = r4 * r4
            //         |    19. r4 *= 19           // r4 = r2 * r4
            //         |    20. r4 *= 11           // r4 = r4 * 11
            //         |    21. ++r5               // r5 = r5 + 1
            //         |    22. r5 *= 22           // r5 = r5 * r2
            //         |    23. r5 += 17           // r5 = r5 + 17
            //         |    24. r4 += r5           // r4 = r4 + r5
            //         |    25. jump +[r0 + 1]     // r2 = r2 + r0
            //         +--- 26. jump 1             // r2 = 0
            //              27. r5 = 27            // r5 = r2
            //              28. r5 *= 28           // r5 = r5 * r2
            //              29. r5 += 29           // r5 = r2 + r5
            //              30. r5 *= 30           // r5 = r2 * r5
            //              31. r5 *= 14           // r5 = r5 * 14
            //              32. r5 *= 32           // r5 = r5 * r2
            //              33. r4 += r5           // r4 = r4 + r5
            //              34. r0 = 0
            //              35. jump 1             // r2 = 0
            //
            //  r4 -> (4*19*11) + (22 + 17)
            //  r4 -> (27 * 28 + 29) * 30 * 14 * 32 + r4
            //
            //  r1 = 1
            //  while (r1 <= r4):
            //    for r3 = 1; r3 <= r4; ++r3:
            //      if r3 * r1 == r4:
            //        r0 += r1
            //

            auto calc = [](int64_t r4) {
                int64_t r0 = 0;

                for (int64_t r1 = 1; r1 <= r4; ++r1)
                {
                    for (int64_t r3 = 1; r3 <= r4; ++r3)
                    {
                        auto m = r3 * r1;
                        if (m == r4)
                        {
                            r0 += r1;
                        }
                        if (m > r4)
                        {
                            break;
                        }
                    }
                }

                return r0;
            };

            int64_t r4 = 4ll * 19 * 11 + 22 + 17;
            expect(1248_i == calc(r4));
            r4 += (27 * 28 + 29) * 30 * 14 * 32;
            Printer::Print(__FILE__, "2", calc(r4));
        }
    };
};

} //namespace;
