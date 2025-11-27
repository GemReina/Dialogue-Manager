#pragma once
#include "../model/DialogueTree.h"

class InspectorPanel {
public:
    // Pasamos selectedNodeId por referencia para poder modificarlo
    void Draw(DialogueTree& tree, int& selectedNodeId);
};
