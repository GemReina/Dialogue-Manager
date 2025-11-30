#define _CRT_SECURE_NO_WARNINGS
#include "ui/EditorApp.h"
#include "io/JsonSerializer.h"
#include "ui/PreviewPanel.h"
#include <imgui.h>
#include "runtime/DialoguePlayer.h"

EditorApp::EditorApp() {
    NewTree("New Dialogue Tree");
}

void EditorApp::NewTree(const std::string& name) {
    tree = DialogueTree(name);
    int n1 = tree.createNode(DialogueNodeType::Basic);
    tree.nodes[n1].setCharacter("Hero");
    tree.nodes[n1].setText("Hello there!");

    int n2 = tree.createNode(DialogueNodeType::Options);
    tree.nodes[n2].setCharacter("Villain");
    tree.nodes[n2].setText("We meet again...");

    tree.connectNodes(n1, n2);
}

void EditorApp::drawMainMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) { NewTree("Untitled"); }
            if (ImGui::MenuItem("Save")) { io::JsonSerializer::Save(tree, "dialogue.json"); }
            if (ImGui::MenuItem("Load")) { io::JsonSerializer::Load(tree, "dialogue.json"); }
            if (ImGui::MenuItem("Exit")) {
                // handled in main loop via window close
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Create Basic Node")) {
                int id = tree.createNode(DialogueNodeType::Basic);
                canvasState.selectedNodeId = id;
            }
            if (ImGui::MenuItem("Create Options Node")) {
                int id = tree.createNode(DialogueNodeType::Options);
                canvasState.selectedNodeId = id;
            }
            ImGui::EndMenu();
        }

        // runtime toggle
        if (ImGui::BeginMenu("Runtime")) {
            ImGui::MenuItem("Runtime Mode", nullptr, &runtimeMode);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void EditorApp::drawDockspace() {
    // Ensure dockspace covers the whole application viewport and updates when window is resized
    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGui::Begin("DockSpaceHost", nullptr, flags);
    ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
    ImGui::DockSpace(dockspace_id);
    ImGui::End();
}

void EditorApp::Frame() {
    drawMainMenu();
    drawDockspace();

    canvas.Draw(tree, canvasState);
    inspector.Draw(tree, canvasState.selectedNodeId); // ← ahora pasa por referencia
    charPanel.Draw(tree);

    // Use PreviewPanel
    static PreviewPanel preview;
    static DialoguePlayer player;
    if (runtimeMode) {
        // if not playing yet, start at root
        if (!player.IsPlaying()) player.Start(tree, tree.rootId);
        preview.DrawRuntime(player);
    } else {
        preview.Draw(tree, canvasState.selectedNodeId);
    }
}
