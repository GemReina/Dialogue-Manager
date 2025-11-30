#pragma once
#include "../model/DialogueTree.h"

// Panel to display and edit character definitions associated with the dialogue tree.
class CharacterPanel {
public:
    // pre-condition: tree is a valid DialogueTree
    // post-condition: renders character editing UI and may modify tree.characters
    void Draw(DialogueTree& tree);
};
