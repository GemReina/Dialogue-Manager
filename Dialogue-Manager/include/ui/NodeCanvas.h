#pragma once
#include "../model/DialogueTree.h"

// Description: Keeps canvas transformation and interaction state (zoom, offset, selection, panning).
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
    // pre-condition: tree is a valid DialogueTree and state is initialized
    // post-condition: renders the canvas UI into the current ImGui window
    void Draw(DialogueTree& tree, CanvasState& state);
private:
    // pre-condition: ImGui window is active
    // post-condition: grid is drawn and an invisible control is created for panning
    void drawGrid();
    // pre-condition: node is a valid DialogueNode
    // post-condition: node is drawn at its world position transformed by state
    void drawNode(DialogueNode& node, CanvasState& state, bool isSelected);
    // pre-condition: node is valid and mx/my are screen coords
    // post-condition: returns true if point hits node rectangle
    bool nodeRectHit(const DialogueNode& node, float mx, float my, const CanvasState& state);
};
