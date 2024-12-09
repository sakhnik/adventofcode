#include "../test.hpp"

namespace {

using namespace boost::ut;

using IntT = int64_t;

std::pair<IntT, IntT> Task(std::istream &&is)
{
    std::string line;
    std::getline(is, line);
    if (0 == (line.size() & 1))
        line.pop_back();

    IntT task1{};

    int block = 0;
    auto add = [&](int count, int id) {
        IntT a = block;
        IntT b = block + count - 1;
        task1 += id * ((a + b) * count / 2);
        block += count;
    };

    int right = line.size() + 1;
    int rightCount = 0;

    auto takeRight = [&](int count) {
        if (!rightCount) {
            right -= 2;
            rightCount = line[right] - '0';
        }
        int available = std::min(count, rightCount);
        rightCount -= available;
        return available;
    };

    bool isFree = false;
    int freeCount = 0;
    for (int left = 0; left < right; ) {
        if (!isFree) {
            add(line[left] - '0', left / 2);
            isFree = true;
            ++left;
            freeCount = line[left] - '0';
            ++left;
        } else {
            int r = takeRight(freeCount);
            add(r, right / 2);
            freeCount -= r;
            isFree = freeCount > 0;
        }
    }
    add(rightCount, right / 2);

    //////// Task2 ////////

    struct File
    {
        int size{};
        int id{};
    };
    std::map<int, File> fs;
    std::map<int, int> freeChunks;
    for (int i = 0, offset = 0; i < line.size(); ++i) {
        int size = line[i] - '0';
        if (0 == (i & 1)) {
            fs[offset] = {size, i / 2};
        } else {
            freeChunks[offset] = size;
        }
        offset += size;
    }

    auto printFs = [&]() {
        int o = 0;
        for (auto &f : fs) {
            for (; o < f.first; ++o)
                std::cerr << ".";
            for (int i = 0; i < f.second.size; ++i, ++o) {
                std::cerr << f.second.id;
            }
        }
        std::cerr << std::endl;
    };

    for (auto r = fs.rbegin(); r != fs.rend(); ) {
        //printFs();
        auto offset = r->first;
        auto file = r->second;
        ++r;

        auto it = std::find_if(freeChunks.begin(), freeChunks.end(),
            [&](const auto &chunk) {
                return chunk.second >= file.size && chunk.first < offset;
            });
        if (it != freeChunks.end() && it->first < offset) {
            fs.erase(offset);
            fs[it->first] = file;
            it->second -= file.size;
            if (it->second) {
                freeChunks[it->first + file.size] = it->second;
            }
            freeChunks.erase(it);
        }
    }

    IntT task2 = std::accumulate(fs.begin(), fs.end(), IntT{},
        [](IntT x, const auto &f) {
            const auto &file = f.second;
            IntT a = f.first;
            IntT b = f.first + file.size - 1;
            return x + file.id * ((a + b) * file.size / 2);
        });
    return {task1, task2};
}

suite s = [] {
    "09"_test = [] {
        const char *const TEST1 = R"(2333133121414131402)";
        auto test = Task(std::istringstream{TEST1});
        expect(1928_i == test.first);
        expect(2858_i == test.second);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
