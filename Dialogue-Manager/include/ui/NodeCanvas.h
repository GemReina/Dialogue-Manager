#pragma once
#include "../model/DialogueTree.h"

struct CanvasState {
    float zoom = 1.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;

    int selectedNodeId = -1;
    int linkFromId = -1;
};

class NodeCanvas {
public:
    void Draw(DialogueTree& tree, CanvasState& state);
private:
    void drawGrid();
    void drawNode(DialogueNode& node, CanvasState& state, bool isSelected);
    bool nodeRectHit(const DialogueNode& node, float mx, float my, const CanvasState& state);
};
