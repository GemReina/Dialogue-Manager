#pragma once
#include <map>
#include <string>
#include "DialogueNode.h"

class DialogueTree {
public:
    std::string name;
    std::map<int, DialogueNode> nodes;
    int nextNodeId = 0;

    DialogueTree(const std::string& treeName);

    int createNode(DialogueNodeType type);
    void connectNodes(int fromId, int toId);
    void printTree() const;
};