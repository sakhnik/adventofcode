#include <doctest/doctest.h>
#include "Cpu.h"
#include <fstream>

namespace {
} //namespace;

TEST_CASE(TEST_NAME)
{
    using namespace aoc2018;

    SUBCASE("task") {
        Cpu cpu(std::ifstream{INPUT});
        //cpu.Print();

        // 0. r4 = 123
        // 1. r4 = r4 & 456
        // 2. r4 = r4 == 72
        // 3. jump +[r4 + 1]            // r2 = r4 + r2
        // 4. jump 1                    // r2 = 0
        // 5. r4 = 0
        // 6. r1 = r4 | 0x10000         // r1 = r4 | 65536
        // 7. r4 = 0xf49de8             // r4 = 16031208
        // 8. r3 = r1 & 0xff            // r3 = r1 & 255
        // 9. r4 += r3                  // r4 = r4 + r3
        // 10. r4 &= 0xffffff           // r4 = r4 & 16777215
        // 11. r4 = r4 * 65899
        // 12. r4 &= 0xffffff           // r4 = r4 & 16777215
        // 13. r3 = 256 > r1
        // 14. jump +[r3 + 1]           // r2 = r3 + r2
        // 15. jump 17                  // r2 = r2 + 1
        // 16. jump 28                  // r2 = 27
        // 17. r3 = 0
        // 18. r5 = r3 + 1
        // 19. r5 *= 256                // r5 = r5 * 256
        // 20. r5 = r5 > r1
        // 21. jump +[r5 + 1]           // r2 = r5 + r2
        // 22. jump 24                  // r2 = r2 + 1
        // 23. jump 26                  // r2 = 25
        // 24. ++r3                     // r3 = r3 + 1
        // 25. jump 18                  // r2 = 17
        // 26. r1 = r3
        // 27. jump 8                   // r2 = 7
        // 28. r3 = r4 == r0
        // 29. jump +[r3 + 1]           // r2 = r3 + r2
        // 30. jump 6                   // r2 = 5

        // Apparently, we can break the program the first time the line 28 is hit
        // for the task1 => the fewest instructions.
 
        while (true)
        {
            cpu.Step();
            if (cpu.GetIp() == 28)
            {
                MESSAGE(cpu.GetRegs()[4]);
                break;
            }
        }
    }
}
