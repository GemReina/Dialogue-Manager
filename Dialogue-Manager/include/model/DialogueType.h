#pragma once
#include <string>
#include <vector>

struct EditableField {
    std::string name;   // e.g., "text", "voicelineRef"
    std::string type;   // "string", "int", "bool", "color", etc.
    std::string defaultValue;
};

class DialogueTypeDef {
public:
    int id = -1;
    std::string name = "Basic";
    std::vector<EditableField> fields;
};
