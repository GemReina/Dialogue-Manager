#pragma once
#include <string>
#include <vector>

// Represents a single editable field in a DialogueType definition (used by the editor to render inputs).
struct EditableField {
    // pre-condition: name and type are valid strings
    // post-condition: field definition stored; defaultValue used when creating nodes of this type
    std::string name;   // e.g., "text", "voicelineRef"
    std::string type;   // "string", "int", "bool", "color", etc.
    std::string defaultValue;
};

// Dialogue type definition (metadata describing custom node fields used by specific dialogue types).
class DialogueTypeDef {
public:
    // pre-condition: none
    // post-condition: DialogueTypeDef created with default values
    int id = -1;
    std::string name = "Basic";
    std::vector<EditableField> fields;
};
