#include "DialogueNode.h"

DialogueNode::DialogueNode(int id, DialogueNodeType type)
    : id(id), type(type) {
}

void DialogueNode::setText(const std::string& newText) {
    text = newText;
}

void DialogueNode::setCharacter(const std::string& charId) {
    characterId = charId;
}
