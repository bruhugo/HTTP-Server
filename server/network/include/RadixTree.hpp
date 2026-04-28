#pragma once

#include "Context.hpp"

#include <string>
#include <vector>

namespace server {
namespace network{

struct RadixTreeNode {
    RadixTreeNode();
    RadixTreeNode(std::string label);

    Handler handler;
    Filters* filters;

    bool wildcard;
    std::string wildcardName;

    // without the '/', so the root node is just ""
    std::string label;
    std::vector<RadixTreeNode*> children;
};

class RadixTree {
public:
    // sets the given handler for the given path.
    // throws if already set
    void addHandler(std::string path, Handler handler);

    // sets filters for the given path
    void addFilter(std::string path, Handler handler);

private:
    RadixTreeNode* root;

    std::vector<std::string> breakPath(std::string& str);
    RadixTreeNode* traverseAndCreate(std::string path);
};

}
}


