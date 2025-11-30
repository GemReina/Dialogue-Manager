#pragma once
#include "../model/DialogueTree.h"

// Description: Inspector panel UI for editing selected DialogueNode properties and connections.
class InspectorPanel {
public:
    // pre-condition: tree is valid, selectedNodeId refers to a node or -1
    // post-condition: Renders inspector UI and may modify tree and selectedNodeId
    void Draw(DialogueTree& tree, int& selectedNodeId);
};
