#include "RadixTree.hpp"

using namespace server::network;

RadixTreeNode::RadixTreeNode(): label{""}{}
RadixTreeNode::RadixTreeNode(std::string l): label{l}{}


void RadixTree::addHandler(std::string path, Handler handler){
    RadixTreeNode* node = traverseAndCreate(path);
    node->handler = handler;
}

void RadixTree::addFilter(std::string path, Handler handler){
    RadixTreeNode* node = traverseAndCreate(path);
    node->filters->add(handler);
}


std::vector<std::string> RadixTree::breakPath(std::string& str){
    std::vector<std::string> paths;
    
    size_t start = 0;
    size_t end = str.find('/');

    while (end != str.npos){
        if (end > start){
            paths.push_back(str.substr(start, end - start));
        }
        start = end + 1;
        end = str.find('/');
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
        cur->children.push_back(newNode);
        cur = newNode;
    }

    return cur;
}