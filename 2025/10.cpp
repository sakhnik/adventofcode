// setlocal makeprg=g++\ -std=c++20\ -g\ -fsanitize=address\ 10.cpp\ -llpsolve55

#include <vector>
#include <iostream>
#include <sstream>
#include <bitset>
#include <cassert>
#include <fstream>
#include <memory>
#include <numeric>
#include <lpsolve/lp_lib.h>

struct Task
{
    int res1{}, res2{};

    using SwitchesT = std::vector<std::vector<int>>;

    Task(std::istream &&is)
    {
        std::string line;
        while (std::getline(is, line)) {
            int n_lights = 0;
            int target_state = 0;
            SwitchesT switches;
            std::vector<int> target_counts;

            std::istringstream iss{line};
            std::string token;
            while (iss >> token) {
                if (token[0] == '[') {
                    token = token.substr(1, token.size() - 2);
                    n_lights = token.size();
                    for (int i = 0; i < n_lights; ++i) {
                        target_state |= (token[i] == '#') << i;
                    }
                } else if (token[0] == '(') {
                    std::istringstream iss{token};
                    std::vector<int> sw;
                    int light;
                    char sep;
                    while (iss >> sep >> light) {
                        sw.push_back(light);
                    }
                    switches.emplace_back(std::move(sw));
                } else if (token[0] == '{') {
                    std::istringstream iss{token};
                    int count;
                    char sep;
                    while (iss >> sep >> count) {
                        target_counts.emplace_back(count);
                    }
                }
            }

            res1 += Find1(n_lights, target_state, switches);
            res2 += Find2(n_lights, target_counts, switches);
        }
    }

    int Find1(int n_lights, int target_state, const SwitchesT &switches)
    {
        std::vector<int> switch_masks;
        for (auto &sw : switches) {
            int mask = 0;
            for (auto i : sw) {
                mask |= (1 << i);
            }
            switch_masks.push_back(mask);
        }

        int N = 1 << switches.size();
        int min = switches.size() + 1;
        for (int toggle_mask = 0; toggle_mask < N; ++toggle_mask) {
            std::bitset<32> b = toggle_mask;
            if (b.count() >= min)
                continue;
            int state = 0;
            for (int i = 0; i < switches.size(); ++i) {
                if ((toggle_mask & (1 << i))) {
                    state ^= switch_masks[i];
                }
            }
            if (state == target_state)
                min = b.count();
        }
        return min;
    }

    int Find2(int n_lights, const std::vector<int> &target_counts, const SwitchesT &switches)
    {
        int n_vars = switches.size();

        // LP: n_vars variables, 0 constraints initially
        std::unique_ptr<lprec, void(*)(lprec *)> lp{make_lp(0, n_vars), delete_lp};
        if (!lp) return -1;
        set_verbose(lp.get(), IMPORTANT);

        // objective: minimize sum x_j
        std::vector<double> col(n_vars + 1, 1.0);
        set_obj_fn(lp.get(), col.data());

        for (int j = 0; j < n_vars; ++j)
            set_int(lp.get(), j + 1, true);

        for (int j = 0; j < n_vars; ++j)
            set_lowbo(lp.get(), j + 1, 0.0);

        for (int i = 0; i < n_lights; ++i) {
            std::vector<double> row(n_vars + 1, 0.0);
            for (int j = 0; j < n_vars; ++j) {
                for (int l : switches[j])
                    if (l == i)
                        row[j + 1] += 1.0;
            }
            add_constraint(lp.get(), row.data(), EQ, static_cast<double>(target_counts[i]));
        }

        int res = solve(lp.get());
        assert(res == OPTIMAL);

        std::vector<double> sol(n_vars);
        get_variables(lp.get(), sol.data());

        return std::round(std::accumulate(sol.begin(), sol.end(), 0.0));
    }
};

int main()
{
    const char *const TEST =
        "[.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}\n"
        "[...#.] (0,2,3,4) (2,3) (0,4) (0,1,2) (1,2,3,4) {7,5,12,7,2}\n"
        "[.###.#] (0,1,2,3,4) (0,3,4) (0,1,2,4,5) (1,2) {10,11,11,5,10,5}";
    Task test{std::istringstream{TEST}};
    assert(test.res1 == 7);
    assert(test.res2 == 33);

    Task task{std::ifstream{"10.txt"}};
    std::cout << task.res1 << std::endl;
    std::cout << task.res2 << std::endl;

    return 0;
}
