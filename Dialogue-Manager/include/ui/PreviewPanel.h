#pragma once
#include "../model/DialogueTree.h"

class PreviewPanel {
public:
    // selectedNodeId is passed by reference so preview can navigate to children
    void Draw(DialogueTree& tree, int& selectedNodeId);
};
