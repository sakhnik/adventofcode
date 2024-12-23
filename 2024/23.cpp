#include "../test.hpp"
#include <fmt/ranges.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bron_kerbosch_all_cliques.hpp>
#include <boost/graph/graph_utility.hpp>

namespace {

using namespace boost::ut;

std::pair<int, std::string> Task(std::istream &&is)
{
    using namespace boost;

    using GraphT = adjacency_list<vecS, vecS, undirectedS>;
    using VertexT = graph_traits<GraphT>::vertex_descriptor;

    GraphT g;

    std::unordered_map<std::string, int> labelVertices;
    std::vector<std::string> labels;
    auto getVertex = [&](std::string label) {
        auto it = labelVertices.find(label);
        if (it != labelVertices.end())
            return it->second;
        labels.push_back(label);
        return labelVertices[label] = labelVertices.size();
    };

    std::string line;
    while (getline(is, line)) {
        std::string a = line.substr(0, 2);
        std::string b = line.substr(3);
        add_edge(getVertex(a), getVertex(b), g);
    }

    using TriangleT = std::array<VertexT, 3>;
    struct TriangleHash
    {
        size_t operator()(const TriangleT &t) const
        {
            size_t h{};
            boost::hash_combine(h, t[0]);
            boost::hash_combine(h, t[1]);
            boost::hash_combine(h, t[2]);
            return h;
        }
    };
    std::unordered_set<TriangleT, TriangleHash> triangles;

    for (auto vi = vertices(g).first; vi != vertices(g).second; ++vi) {
        for (auto ai = adjacent_vertices(*vi, g).first; ai != adjacent_vertices(*vi, g).second; ++ai) {
            for (auto aj = ai; aj != adjacent_vertices(*vi, g).second; ++aj) {
                if (edge(*ai, *aj, g).second) {
                    TriangleT t{*vi, *ai, *aj};
                    std::sort(t.begin(), t.end());
                    triangles.insert(t);
                }
            }
        }
    }

    int task1{};
    for (const auto &t : triangles) {
        auto it = std::find_if(t.begin(), t.end(), [&](auto v) { return labels[v].starts_with('t'); });
        if (it != t.end())
            ++task1;
    }

    struct CliqueVisitor
    {
        std::string &pass;
        const std::vector<std::string> &labels;

        CliqueVisitor(std::string &pass, const std::vector<std::string> &labels)
            : pass{pass}
            , labels{labels}
        {
        }

        void clique(const std::deque<VertexT> &clique, const GraphT &g)
        {
            std::vector<std::string> n;
            for (auto v : clique) {
                n.push_back(labels[v]);
            }
            std::sort(n.begin(), n.end());
            auto p = fmt::format("{}", fmt::join(n, ","));
            if (p.size() > pass.size())
                pass = p;
        }
    };

    std::string task2;
    bron_kerbosch_all_cliques(g, CliqueVisitor{task2, labels});

    return {task1, task2};
}

suite s = [] {
    "23"_test = [] {
        const char *const TEST1 = R"(kh-tc
qp-kh
de-cg
ka-co
yn-aq
qp-ub
cg-tb
vc-aq
tb-ka
wh-tc
yn-cg
kh-ub
ta-co
de-co
tc-td
tb-wq
wh-td
ta-ka
td-qp
aq-cg
wq-ub
ub-vc
de-ta
wq-aq
wq-vc
wh-yn
ka-de
kh-ta
co-tc
wh-qp
tb-vc
td-yn)";
        auto test = Task(std::istringstream{TEST1});
        expect(7_i == test.first);
        expect(eq(std::string{"co,de,ka,ta"}, test.second));

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
