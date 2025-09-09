#pragma once
#include <string>
#include <vector>

enum class DialogueNodeType{
    Basic,
    Options
};

class DialogueNode{
public:
    int id;
    DialogueNodeType type;
    std::string characterId;
    std::string text;
    std::vector<int> children; // connections to other nodes

    DialogueNode() = default; //default constructor
    DialogueNode(int id, DialogueNodeType type);

    void setText(const std::string& newText);
    void setCharacter(const std::string& charId);
};