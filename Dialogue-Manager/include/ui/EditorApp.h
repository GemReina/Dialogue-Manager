#pragma once
#include "../model/DialogueTree.h"
#include "NodeCanvas.h"
#include "InspectorPanel.h"
#include "CharacterPanel.h"
#include <string>

class EditorApp {
public:
    EditorApp();

    void Frame();
    void NewTree(const std::string& name);
    DialogueTree& GetTree() { return tree; }

private:
    DialogueTree tree;
    CanvasState canvasState;
    NodeCanvas canvas;
    InspectorPanel inspector;
    CharacterPanel charPanel;

    void drawMainMenu();
    void drawDockspace();
};
