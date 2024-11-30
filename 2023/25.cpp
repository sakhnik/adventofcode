#include "../test.hpp"
#include <fstream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/stoer_wagner_min_cut.hpp>

namespace {

using namespace boost::ut;

struct Partition
{
    using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
    Graph graph;
    std::unordered_map<std::string, int> vertices;

    int GetVertex(const std::string &label)
    {
        auto it = vertices.find(label);
        if (it != vertices.end())
            return it->second;
        boost::add_vertex(graph);
        return vertices[label] = vertices.size();
    }

    Partition(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            std::istringstream iss{line};
            std::string label;
            iss >> label;
            label.pop_back();
            int u = GetVertex(label);
            while (iss >> label)
                boost::add_edge(u, GetVertex(label), graph);
        }
    }


    int Task1()
    {
        const auto parities = boost::make_one_bit_color_map(boost::num_vertices(graph), boost::get(boost::vertex_index, graph));
        const auto parity_map = boost::parity_map(parities);

        /*int cut_weight =*/ boost::stoer_wagner_min_cut(graph, boost::make_static_property_map<Graph::edge_descriptor>(1), parity_map);
        int count{};
        for (size_t i = 0; i < boost::num_vertices(graph); ++i) {
            if (boost::get(parities, i))
                ++count;
        }
        return count * (boost::num_vertices(graph) - count);
    }

    std::string Task2()
    {
        return 0;
    }
};

suite s = [] {
    "25"_test = [] {
        const char *const TEST1 = R"(jqt: rhn xhk nvd
rsh: frs pzl lsr
xhk: hfx
cmg: qnr nvd lhk bvb
rhn: xhk bvb hfx
bvb: xhk hfx
pzl: lsr hfx nvd
qnr: nvd
ntq: jqt hfx bvb xhk
nvd: lhk
lsr: lhk
rzs: qnr cmg lsr rsh
frs: qnr lhk lsr
)";
        Partition test1{std::istringstream{TEST1}};
        expect(54_i == test1.Task1());

        if (Printer::EnableAll())
        {
            Partition task{std::ifstream{INPUT}};
            Printer::Print(__FILE__, "1", task.Task1());
        }
    };
};

} //namespace;
