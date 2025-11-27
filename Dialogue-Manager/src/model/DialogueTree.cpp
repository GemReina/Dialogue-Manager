#include "model/DialogueTree.h"
#include <iostream>
#include <algorithm>

DialogueTree::DialogueTree(const std::string& treeName)
    : name(treeName) {
}

int DialogueTree::createNode(DialogueNodeType type) {
    int id = nextNodeId++;
    DialogueNode node(id, type);
    // simple layout hint
    node.x = float((id % 5) * 200);
    node.y = float((id / 5) * 140);
    nodes.emplace(id, std::move(node));
    if (rootId == -1) rootId = id;
    return id;
}

void DialogueTree::removeNode(int id) {
    nodes.erase(id);
    for (auto& [nid, n] : nodes) {
        n.children.erase(std::remove(n.children.begin(), n.children.end(), id), n.children.end());
    }
    if (rootId == id) rootId = nodes.empty() ? -1 : nodes.begin()->first;
}

void DialogueTree::connectNodes(int fromId, int toId) {
    if (nodes.count(fromId) && nodes.count(toId)) {
        auto& v = nodes[fromId].children;
        if (std::find(v.begin(), v.end(), toId) == v.end())
            v.push_back(toId);
    }
}

void DialogueTree::disconnectNodes(int fromId, int toId) {
    if (nodes.count(fromId)) {
        auto& v = nodes[fromId].children;
        v.erase(std::remove(v.begin(), v.end(), toId), v.end());
    }
}

void DialogueTree::setRoot(int id) {
    if (nodes.count(id)) rootId = id;
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
            for (int child : node.children) std::cout << child << " ";
            std::cout << "\n";
        }
    }
}
