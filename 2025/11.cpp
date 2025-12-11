#include <cstdint>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <cassert>
#include <fstream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/strong_components.hpp>

struct Task
{
    uint64_t res1{}, res2{};

    Task(std::istream &&is)
    {
        using GraphT = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS>;
        using VertexT = GraphT::vertex_descriptor;

        GraphT g;
        std::unordered_map<std::string, VertexT> labels;

        auto get = [&](const std::string &s) -> VertexT {
            auto it = labels.find(s);
            if (it != labels.end())
                return it->second;
            auto v = add_vertex(g);
            return labels[s] = v;
        };

        std::string line;
        while (std::getline(is, line)) {
            std::istringstream iss{line};
            std::string v;
            iss >> v;
            auto u = get(v.substr(0, v.size() - 1));
            while (iss >> v) {
                add_edge(u, get(v), g);
            }
        }

        auto count_paths = [](const GraphT &g, VertexT u, VertexT dst, std::vector<int64_t> &memo, auto &&count_paths) -> int64_t {
            if (u == dst)
                return 1;
            if (memo[u] != -1)
                return memo[u];

            int64_t count = 0;
            for (auto [it, it_end] = out_edges(u, g); it != it_end; ++it) {
                count += count_paths(g, target(*it, g), dst, memo, count_paths);
            }
            return memo[u] = count;
        };

        VertexT you = get("you");
        VertexT out = get("out");

        {
            std::vector<int64_t> memo(num_vertices(g), -1);
            res1 = count_paths(g, you, out, memo, count_paths);
        }

        VertexT svr = get("svr");
        VertexT dac = get("dac");
        VertexT fft = get("fft");

        const int N = num_vertices(g);

        std::vector<VertexT> comp(N);
        int num_scc = strong_components(g, comp.data());

        GraphT dag(num_scc);
        for (size_t u = 0; u < N; ++u) {
            for (auto e : boost::make_iterator_range(out_edges(u, g))) {
                size_t v = target(e, g);
                int cu = comp[u];
                int cv = comp[v];
                if (cu != cv) {
                    add_edge(cu, cv, dag);
                }
            }
        }

        auto count_segment = [&](int src, int dst) {
            std::vector<int64_t> memo(num_scc, -1);
            return count_paths(dag, src, dst, memo, count_paths);
        };

        res2 = count_segment(comp[svr], comp[dac])
             * count_segment(comp[dac], comp[fft])
             * count_segment(comp[fft], comp[out])
 
             + count_segment(comp[svr], comp[fft])
             * count_segment(comp[fft], comp[dac])
             * count_segment(comp[dac], comp[out]);
    }
};

int main()
{
    const char *const TEST1 =
        "aaa: you hhh\n"
        "you: bbb ccc\n"
        "bbb: ddd eee\n"
        "ccc: ddd eee fff\n"
        "ddd: ggg\n"
        "eee: out\n"
        "fff: out\n"
        "ggg: out\n"
        "hhh: ccc fff iii\n"
        "iii: out";
    Task test1{std::istringstream{TEST1}};
    assert(test1.res1 == 5);

    const char *const TEST2 =
        "svr: aaa bbb\n"
        "aaa: fft\n"
        "fft: ccc\n"
        "bbb: tty\n"
        "tty: ccc\n"
        "ccc: ddd eee\n"
        "ddd: hub\n"
        "hub: fff\n"
        "eee: dac\n"
        "dac: fff\n"
        "fff: ggg hhh\n"
        "ggg: out\n"
        "hhh: out";
    Task test2{std::istringstream{TEST2}};
    assert(test2.res2 == 2);

    Task task{std::ifstream{"11.txt"}};
    std::cout << task.res1 << std::endl;
    std::cout << task.res2 << std::endl;
    return 0;
}
