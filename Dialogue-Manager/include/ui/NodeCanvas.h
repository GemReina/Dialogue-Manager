#pragma once
#include "../model/DialogueTree.h"

struct CanvasState {
    float zoom = 1.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;

    int selectedNodeId = -1;
    int linkFromId = -1;

    // Editor interaction state
    bool panning = false; // true when user is dragging the background to pan
    float panStartMouseX = 0.0f; // stored mouse position when panning started
    float panStartMouseY = 0.0f;
    float panStartOffsetX = 0.0f; // stored offsets when panning started
    float panStartOffsetY = 0.0f;
};

class NodeCanvas {
public:
    void Draw(DialogueTree& tree, CanvasState& state);
private:
    void drawGrid();
    void drawNode(DialogueNode& node, CanvasState& state, bool isSelected);
    bool nodeRectHit(const DialogueNode& node, float mx, float my, const CanvasState& state);
};
