#pragma once
#include <string>
#include <vector>
#include <unordered_map>

// Type of dialogue node. Basic is single text; Options contains multiple selectable options.
enum class DialogueNodeType {
    Basic = 0,
    Options = 1
};

// A condition that gates transitions or option visibility.
struct Condition {
    std::string key;
    std::string op; // "==", "!=", ">", "<", ">=", "<="
    std::string value;
};

// Event triggered when node is activated; contains a name and parameters.
struct Event {
    std::string name;
    std::unordered_map<std::string, std::string> params;
};

// Single option in an Options node, with displayed text and a child node id.
struct Option {
    std::string text;
    int childId = -1; // id of the child node for this option, -1 = none
};

// Represents a dialogue node in the editor, containing text, character, outgoing edges and editor layout info.
class DialogueNode {
public:
    int id = -1;
    DialogueNodeType type = DialogueNodeType::Basic;

    std::string characterId;
    std::string text;            // For now; later replace with textId for localization
    std::string voicelineRef;    // Optional audio asset ref
    int affinityDelta = 0;

    int uiStyleId = -1;

    std::vector<int> children;       // edges to child node ids (legacy / general-purpose)
    std::vector<Option> options;     // option entries for Options node type
    std::vector<Condition> conditions;
    std::vector<Event> events;

    // Editor state (not exported to runtime)
    float x = 0.0f;
    float y = 0.0f;

    // pre-condition: default constructible
    // post-condition: DialogueNode created with default values
    DialogueNode() = default;

    // pre-condition: valid id and type provided
    // post-condition: DialogueNode created with id and type set, other fields defaulted
    DialogueNode(int id, DialogueNodeType type);

    // pre-condition: newText is a valid string
    // post-condition: node text is updated
    void setText(const std::string& newText);

    // pre-condition: charId is a valid character id string
    // post-condition: node.characterId updated
    void setCharacter(const std::string& charId);
};
