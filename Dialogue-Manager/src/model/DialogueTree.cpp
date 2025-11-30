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
    std::cout << "Created node " << id << " type=" << (type == DialogueNodeType::Basic ? "Basic" : "Options") << "\n";
    return id;
}

void DialogueTree::removeNode(int id) {
    nodes.erase(id);
    for (auto& [nid, n] : nodes) {
        n.children.erase(std::remove(n.children.begin(), n.children.end(), id), n.children.end());
        // Also clear option references to the removed node
        for (auto& opt : n.options) {
            if (opt.childId == id) opt.childId = -1;
        }
    }
    if (rootId == id) rootId = nodes.empty() ? -1 : nodes.begin()->first;
    std::cout << "Removed node " << id << "\n";
}

void DialogueTree::connectNodes(int fromId, int toId) {
    if (nodes.count(fromId) && nodes.count(toId)) {
        auto& v = nodes[fromId].children;
        if (std::find(v.begin(), v.end(), toId) == v.end())
            v.push_back(toId);
        std::cout << "Connected node " << fromId << " -> " << toId << "\n";
    } else {
        std::cout << "Failed to connect " << fromId << " -> " << toId << " (missing node)\n";
    }
}

void DialogueTree::disconnectNodes(int fromId, int toId) {
    if (nodes.count(fromId)) {
        auto& v = nodes[fromId].children;
        v.erase(std::remove(v.begin(), v.end(), toId), v.end());
        // Clear any option entries that referenced this child
        auto& opts = nodes[fromId].options;
        for (auto& opt : opts) {
            if (opt.childId == toId) opt.childId = -1;
        }
        std::cout << "Disconnected node " << fromId << " -> " << toId << "\n";
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
        if (!node.options.empty()) {
            std::cout << "   Options:";
            for (const auto& o : node.options) std::cout << " (" << o.text << " -> " << o.childId << ")";
            std::cout << "\n";
        }
    }
}
