#include "RadixTree.hpp"

#include <stdexcept>

using namespace server::network;

RadixTreeNode::RadixTreeNode(): label{""}, wildcard{false}, children(0){
    filters = new Filters();
}
RadixTreeNode::RadixTreeNode(std::string l): label{l}, wildcard{false}, children(0){
    filters = new Filters();
}

RadixTreeNode::~RadixTreeNode(){
    delete filters;
    for (auto child : children){
        delete child;
    }
}

RadixTree::RadixTree(){
    root = new RadixTreeNode();
}

RadixTree::~RadixTree(){
    delete root;
}


void RadixTree::addHandler(Method method, std::string path, Handler handler){
    RadixTreeNode* node = traverseAndCreate(path);
    node->handlers[static_cast<size_t>(method)] = handler;
}

void RadixTree::addFilter(std::string path, Handler handler){
    RadixTreeNode* node = traverseAndCreate(path);
    node->filters->add(handler);
}


std::vector<std::string> RadixTree::breakPath(std::string& str){
    std::vector<std::string> paths;
    
    size_t start = 0;
    size_t end = str.find('/', start);

    while (end != str.npos){
        if (end > start){
            paths.push_back(str.substr(start, end - start));
        }
        start = end + 1;
        end = str.find('/', start);
    }

    if (start < str.npos){
        paths.push_back(str.substr(start));
    }

    return paths;
}

RadixTreeNode* RadixTree::traverseAndCreate(std::string path){
    auto paths = breakPath(path);

    RadixTreeNode* cur = root;
    for (std::string path : paths){
        if (path.empty()) continue;
        bool found = false;
        for (auto child : cur->children){
            if (child->label == path){
                cur = child;
                found = true;
                break;
            }
        }

        if (found) continue;

        RadixTreeNode* newNode = new RadixTreeNode(path);

        if (path.at(0) == ':'){
            newNode->wildcard = true;
            newNode->wildcardName = path.substr(1, path.size() - 1);
        }

        cur->children.push_back(newNode);
        cur = newNode;
    }

    return cur;
}

enum class MatchType {
    NONE,
    EXACT,
    WILDCARD,
};

struct MatchChildren {
    RadixTreeNode* node;
    MatchType matchType;

    explicit operator bool() const noexcept{
        return node != nullptr;
    }
};

static MatchChildren findChildren(RadixTreeNode* parent, std::string_view path){
    MatchChildren match{nullptr, MatchType::NONE};

    for (RadixTreeNode* node : parent->children){
        if (node->wildcard){
            match.matchType = MatchType::WILDCARD;
            match.node = node;
            continue;
        }

        if (node->label != path) continue;

        match.matchType = MatchType::EXACT;
        match.node = node;
        return match;
    }

    return match; 
}

RadixQueryResult RadixTree::query(Method method, std::string path){
    Filters filters;
    Params params;

    std::vector<std::string> paths = breakPath(path);

    RadixTreeNode* curNode = root; 
    filters.add(curNode->filters);

    for (std::string curPath : paths){
        MatchChildren match = findChildren(curNode, curPath);
        if (!match)
            // TODO: make it HTTP error later
            throw std::runtime_error("resource not found");

        if (match.matchType == MatchType::WILDCARD)
            params.emplace(match.node->wildcardName, curPath);
        
        filters.add(match.node->filters);
        filters.add(match.node->handlers[static_cast<size_t>(method)]);
        curNode = match.node;
    }

    return {params, filters};
}
