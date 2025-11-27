#pragma once
#include "DialogueNode.h"
#include "Character.h"
#include "UiStyle.h"
#include "DialogueType.h"

#include <unordered_map>
#include <string>
#include <vector>

class DialogueTree {
public:
    std::string name;
    int nextNodeId = 1;

    std::unordered_map<int, DialogueNode> nodes;
    std::unordered_map<std::string, Character> characters;
    std::unordered_map<int, UiStyle> styles;
    std::unordered_map<int, DialogueTypeDef> typeDefs;

    int rootId = -1;

    DialogueTree() = default;
    explicit DialogueTree(const std::string& treeName);

    int createNode(DialogueNodeType type);
    void removeNode(int id);

    void connectNodes(int fromId, int toId);
    void disconnectNodes(int fromId, int toId);

    void setRoot(int id);

    void printTree() const;
};
