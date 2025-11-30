#pragma once
#include "DialogueNode.h"
#include "Character.h"
#include "UiStyle.h"
#include "DialogueType.h"

#include <unordered_map>
#include <string>
#include <vector>

// Represents a dialogue graph containing nodes, characters, ui styles and type definitions.
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

    // pre-condition: treeName is a valid string
    // post-condition: DialogueTree constructed with provided name
    explicit DialogueTree(const std::string& treeName);

    // pre-condition: type is a valid DialogueNodeType
    // post-condition: a new node is created in the tree and its id is returned
    int createNode(DialogueNodeType type);

    // pre-condition: id exists in tree.nodes
    // post-condition: node removed and references to it cleared from other nodes
    void removeNode(int id);

    // pre-condition: both node ids exist
    // post-condition: an edge from fromId to toId is added (if not present)
    void connectNodes(int fromId, int toId);

    // pre-condition: fromId exists
    // post-condition: removes edge from fromId to toId if present and clears option references
    void disconnectNodes(int fromId, int toId);

    // pre-condition: id exists in nodes
    // post-condition: sets the root node id
    void setRoot(int id);

    // pre-condition: none
    // post-condition: prints the current tree structure to stdout for debugging
    void printTree() const;
};
