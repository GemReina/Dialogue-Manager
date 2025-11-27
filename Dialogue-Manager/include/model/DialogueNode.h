#pragma once
#include <string>
#include <vector>
#include <unordered_map>

enum class DialogueNodeType {
    Basic = 0,
    Options = 1
};

struct Condition {
    std::string key;
    std::string op; // "==", "!=", ">", "<", ">=", "<="
    std::string value;
};

struct Event {
    std::string name;
    std::unordered_map<std::string, std::string> params;
};

class DialogueNode {
public:
    int id = -1;
    DialogueNodeType type = DialogueNodeType::Basic;

    std::string characterId;
    std::string text;            // For now; later replace with textId for localization
    std::string voicelineRef;    // Optional audio asset ref
    int affinityDelta = 0;

    int uiStyleId = -1;

    std::vector<int> children;       // edges to child node ids
    std::vector<Condition> conditions;
    std::vector<Event> events;

    // Editor state (not exported to runtime)
    float x = 0.0f;
    float y = 0.0f;

    DialogueNode() = default;
    DialogueNode(int id, DialogueNodeType type);

    void setText(const std::string& newText);
    void setCharacter(const std::string& charId);
};
