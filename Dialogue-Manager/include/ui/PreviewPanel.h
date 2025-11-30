#pragma once
#include "../model/DialogueTree.h"

// Preview panel that renders the currently selected node and lets you navigate children for quick runtime checks.
class PreviewPanel {
public:
    // pre-condition: tree is valid and selectedNodeId references a node
    // post-condition: Renders a small preview UI, may update selectedNodeId when navigating
    void Draw(DialogueTree& tree, int& selectedNodeId);
    // Draw using a DialoguePlayer runtime to preview branching and conditions
    void DrawRuntime(class DialoguePlayer& player);
};
