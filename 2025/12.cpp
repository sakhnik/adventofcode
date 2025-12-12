#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <fstream>

struct Task
{
    int res1 = 0;

    Task(std::istream &&is)
    {
        std::vector<int> shape_sizes;

        std::string line;
        while (getline(is, line)) {
            if (line.empty())
                continue;
            if (line.find('x') != line.npos)
                break;
            if (line.back() == ':')
                shape_sizes.push_back(0);
            shape_sizes.back() += std::count(line.begin(), line.end(), '#');
        }

        do {
            std::istringstream iss{line};
            int w, h;
            char ch;
            iss >> w >> ch >> h >> ch;

            int req_area = 0, cnt = 0;
            for (int i = 0; iss >> cnt; ++i) {
                req_area += shape_sizes[i] * cnt;
            }

            res1 += req_area <= w * h;
        } while (getline(is, line));
    }
};

int main()
{
    Task task{std::ifstream{"12.txt"}};
    std::cout << task.res1 << std::endl;

    return 0;
}
