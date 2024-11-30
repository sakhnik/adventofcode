#include <fstream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/connected_components.hpp>
#include "../test.hpp"

namespace {

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> GraphT;

GraphT ReadGraph(std::istream &is)
{
    GraphT graph;

    std::string line;
    while (getline(is, line))
    {
        std::istringstream iss(line);

        unsigned v{0};
        iss >> v;

        std::string skip;
        iss >> skip;  // <->

        unsigned w;
        while (iss >> w)
        {
            add_edge(v, w, graph);
            char ch{0};
            iss >> ch;  // ,
        }
    }

    return graph;
}

GraphT ReadGraph(std::istream &&is)
{
    return ReadGraph(is);
}

class BfsCountingVisitor
    : public boost::default_bfs_visitor
{
public:
    BfsCountingVisitor(unsigned &count) : _count(count) { }

    template <typename Vertex, typename Graph>
        void discover_vertex(Vertex, const Graph &) const
    {
        ++_count;
    }

    unsigned &_count;
};

unsigned Count(const GraphT &graph)
{
    unsigned count{0};
    BfsCountingVisitor vis(count);
    breadth_first_search(graph, 0, visitor(vis));
    return count;
}

using namespace boost::ut;

suite s = [] {
    "12"_test = [] {
        auto test_graph = ReadGraph(std::istringstream(R"(0 <-> 2
1 <-> 1
2 <-> 0, 3, 4
3 <-> 2, 4
4 <-> 2, 3, 6
5 <-> 6
6 <-> 4, 5)"));
        expect(6_u == Count(test_graph));

        std::ifstream ifs(INPUT);
        auto graph = ReadGraph(ifs);
        Printer::Print(__FILE__, "1", Count(graph));

        std::vector<int> component(num_vertices(graph));
        Printer::Print(__FILE__, "2", connected_components(graph, &component[0]));
    };
};

} //namespace;
