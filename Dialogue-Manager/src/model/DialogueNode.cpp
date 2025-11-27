#include "model/DialogueNode.h"

DialogueNode::DialogueNode(int id_, DialogueNodeType type_)
    : id(id_), type(type_) {
}

void DialogueNode::setText(const std::string& newText) {
    text = newText;
}

void DialogueNode::setCharacter(const std::string& charId) {
    characterId = charId;
}
