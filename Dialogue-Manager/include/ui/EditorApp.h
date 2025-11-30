#pragma once
#include "../model/DialogueTree.h"
#include "NodeCanvas.h"
#include "InspectorPanel.h"
#include "CharacterPanel.h"
#include <string>

// Top-level editor application orchestrating panels and the dialogue tree.
class EditorApp {
public:
    // pre-condition: none
    // post-condition: EditorApp initialized with a default new tree
    EditorApp();

    // pre-condition: none
    // post-condition: Renders one frame of the editor UI
    void Frame();
    // pre-condition: name is a valid string
    // post-condition: resets tree to a new tree with the provided name
    void NewTree(const std::string& name);
    DialogueTree& GetTree() { return tree; }

private:
    DialogueTree tree;
    CanvasState canvasState;
    NodeCanvas canvas;
    InspectorPanel inspector;
    CharacterPanel charPanel;

    // runtime preview/player
    bool runtimeMode = false;

    // pre-condition: none
    // post-condition: draws the main menu bar
    void drawMainMenu();
    // pre-condition: none
    // post-condition: draws the dockspace hosting panels
    void drawDockspace();
};
