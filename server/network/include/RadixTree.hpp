#pragma once

#include "Context.hpp"
#include "Types.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace server {
namespace network{


struct RadixTreeNode {
    RadixTreeNode();
    RadixTreeNode(std::string label);

    // use enum method values to map methods to handlers
    Handler handlers[static_cast<size_t>(Method::NUMBER)];
    Filters* filters;

    bool wildcard;
    std::string wildcardName;

    // without the '/', so the root node is just ""
    std::string label;
    std::vector<RadixTreeNode*> children;
};

struct RadixQueryResult {
    Params params;
    Filters filter;
};

class RadixTree {
public:
    // sets the given handler for the given path.
    // throws if already set
    void addHandler(Method method, std::string path, Handler handler);

    // sets filters for the given path
    void addFilter(std::string path, Handler handler);

    RadixQueryResult query(Method method, std::string path);

private:
    RadixTreeNode* root;

    std::vector<std::string> breakPath(std::string& str);
    RadixTreeNode* traverseAndCreate(std::string path);
};

}
}


