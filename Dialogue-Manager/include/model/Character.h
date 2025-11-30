#pragma once
#include <string>

// Represents a character definition used in dialogues (id, display name, portrait, affinity base, default UI style).
class Character {
public:
    // pre-condition: None. Character fields are default-initialized.
    // post-condition: Character instance created with default values; fields may be modified by editor or loader.
    std::string id;
    std::string displayName;
    int affinityBase = 0;
    std::string portraitRef;
    int defaultUiStyleId = -1;
};
