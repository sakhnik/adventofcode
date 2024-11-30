#include "../test.hpp"
#include <fstream>

namespace {

struct Node
{
    using PtrT = std::unique_ptr<Node>;

    Node *parent = nullptr;
    std::unordered_map<std::string, PtrT> children;
    size_t size = 0;

    Node() { }
    Node(size_t size) : size{size} { }
};

void CalcDirSizes(Node *root)
{
    size_t size{};
    for (auto &[name, node] : root->children)
    {
        if (node->parent)
            CalcDirSizes(node.get());
        size += node->size;
    }
    root->size = size;
}

Node::PtrT ParseLog(std::istream &&is)
{
    Node::PtrT root{new Node};
    Node *cur = root.get();

    char buf[64];

    std::string line;
    while (std::getline(is, line))
    {
        if (1 == sscanf(line.c_str(), "$ cd %s", buf))
        {
            if (std::string_view{buf} == "/")
                cur = root.get();
            else if (std::string_view{buf} == "..")
                cur = cur->parent;
            else
            {
                Node::PtrT child{new Node};
                Node *ch = child.get();
                child->parent = cur;
                cur->children[buf] = std::move(child);
                cur = ch;
            }
        }
        else if (line == "$ ls")
        {
            while (is && is.peek() != '$')
            {
                std::getline(is, line);
                size_t size{};
                if (2 == sscanf(line.c_str(), "%ld %s", &size, buf))
                    cur->children[buf].reset(new Node{size});
                else if (1 == sscanf(line.c_str(), "dir %s", buf))
                {
                }
            }
        }
    }
    CalcDirSizes(root.get());
    return root;
}

/*
void Print(Node *node, int depth)
{
    auto indent = [depth] {
        for (int i = 0; i < depth; ++i)
            std::cout << " ";
    };

    for (auto &[name, node] : node->children)
    {
        indent();
        std::cout << name;
        if (node->parent)
        {
            std::cout << "-" << std::endl;
            Print(node.get(), depth + 1);
        }
        else
            std::cout << ": " << node->size << std::endl;
    }
}
*/

size_t Task1(Node *root)
{
    size_t size{};
    auto walk = [&](Node *n, auto &&walk) -> void{
        for (auto &[name, node] : n->children)
        {
            if (node->parent)
            {
                if (node->size <= 100000)
                    size += node->size;
                walk(node.get(), walk);
            }
        }
    };
    walk(root, walk);
    return size;
}

size_t Task2(Node *root)
{
    ssize_t ds = 30000000;
    size_t to_free = ds - (70000000 - root->size);
    size_t closest_size = {};

    auto walk = [&](Node *n, auto &&walk) -> void{
        for (auto &[name, node] : n->children)
        {
            if (node->parent)
            {
                if (node->size >= to_free)
                {
                    ssize_t d = node->size - to_free;
                    if (d < ds)
                    {
                        closest_size = node->size;
                        ds = d;
                    }
                }

                walk(node.get(), walk);
            }
        }
    };
    walk(root, walk);
    return closest_size;
}

const char *const TEST = R"($ cd /
$ ls
dir a
14848514 b.txt
8504156 c.dat
dir d
$ cd a
$ ls
dir e
29116 f
2557 g
62596 h.lst
$ cd e
$ ls
584 i
$ cd ..
$ cd ..
$ cd d
$ ls
4060174 j
8033020 d.log
5626152 d.ext
7214296 k
)";

using namespace boost::ut;

suite s = [] {
    "07"_test = [] {
        auto test_root = ParseLog(std::istringstream{TEST});
        expect(95437_u == Task1(test_root.get()));
        expect(24933642_u == Task2(test_root.get()));

        auto root = ParseLog(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", Task1(root.get()));
        Printer::Print(__FILE__, "2", Task2(root.get()));
    };
};

} //namespace;
