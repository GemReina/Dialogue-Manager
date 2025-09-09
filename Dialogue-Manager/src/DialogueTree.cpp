#include "../include/DialogueTree.h"

#include <iostream>

DialogueTree::DialogueTree(const std::string& treeName)
    : name(treeName) {
}

int DialogueTree::createNode(DialogueNodeType type) {
    int id = nextNodeId++;
    nodes.emplace(id, DialogueNode(id, type));
    return id;
}

void DialogueTree::connectNodes(int fromId, int toId) {
    if (nodes.count(fromId) && nodes.count(toId)) {
        nodes[fromId].children.push_back(toId);
    }
}

void DialogueTree::printTree() const {
    std::cout << "Tree: " << name << "\n";
    for (const auto& [id, node] : nodes) {
        std::cout << " Node " << id << " ("
            << (node.type == DialogueNodeType::Basic ? "Basic" : "Options")
            << ") - Character: " << node.characterId
            << "\n   Text: " << node.text << "\n";

        if (!node.children.empty()) {
            std::cout << "   Children: ";
            for (int child : node.children) {
                std::cout << child << " ";
            }
            std::cout << "\n";
        }
    }
}