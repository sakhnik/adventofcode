#include "../test.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/topological_sort.hpp>
#include <ranges>

namespace {

using namespace boost::ut;

using RulesT = std::vector<std::pair<int, int>>;
using PagesT = std::vector<int>;
using OrderT = std::unordered_map<int, int>;

OrderT GetOrder(const RulesT &rules, const PagesT &pages)
{
    using namespace boost;
    using GraphT = adjacency_list<vecS, vecS, directedS>;
    using VertexT = graph_traits<GraphT>::vertex_descriptor;

    GraphT g;
    std::unordered_map<int, VertexT> vertexMap;
    std::vector<int> vertexIds;

    auto getVertex = [&](int id) -> VertexT {
        auto it = vertexMap.find(id);
        if (it != vertexMap.end()) {
            return it->second;
        }
        vertexIds.push_back(id);
        return vertexMap[id] = add_vertex(g);
    };

    for (auto [a, b] : rules) {
        if (std::find(pages.begin(), pages.end(), a) != pages.end() &&
                std::find(pages.begin(), pages.end(), b) != pages.end()) {
            add_edge(getVertex(a), getVertex(b), g);
        }
    }

    //print_graph(g, std::cerr);

    std::vector<VertexT> topoOrder;
    topological_sort(g, std::back_inserter(topoOrder));

    OrderT order;
    for (auto v : std::ranges::reverse_view(topoOrder)) {
        order[vertexIds[v]] = order.size();
    }

    return order;
}

std::pair<int, int> Task(std::istream &&is)
{
    RulesT rules;
    std::string line;
    while (getline(is, line)) {
        if (line.empty())
            break;
        int a{}, b{};
        sscanf(line.c_str(), "%d|%d", &a, &b);
        rules.emplace_back(a, b);
    }

    int task1{}, task2{};

    while (getline(is, line)) {
        std::vector<int> pages;
        std::istringstream iss{line};
        std::string page;
        while (getline(iss, page, ',')) {
            pages.push_back(std::stoi(page));
        }

        auto order = GetOrder(rules, pages);
        auto comp = [&](int a, int b) { return order[a] < order[b]; };
        bool isOrdered = std::is_sorted(pages.begin(), pages.end(), comp);
        if (isOrdered) {
            task1 += pages[pages.size() / 2];
        } else {
            std::sort(pages.begin(), pages.end(), comp);
            task2 += pages[pages.size() / 2];
        }
    }

    return {task1, task2};
}

suite s = [] {
    "05"_test = [] {
        const char *const TEST1 = R"(47|53
97|13
97|61
97|47
75|29
61|13
75|53
29|13
97|29
53|29
61|53
97|53
61|29
47|13
75|47
97|75
47|61
75|61
47|29
75|13
53|13

75,47,61,53,29
97,61,53,29,13
75,29,13
75,97,47,61,53
61,13,29
97,13,75,29,47)";
        auto test = Task(std::istringstream{TEST1});
        expect(143_i == test.first);
        expect(123_i == test.second);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
