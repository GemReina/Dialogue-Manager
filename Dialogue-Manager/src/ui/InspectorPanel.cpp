#define _CRT_SECURE_NO_WARNINGS
#include "ui/InspectorPanel.h"
#include <imgui.h>
#include <cstring>

void InspectorPanel::Draw(DialogueTree& tree, int& selectedNodeId) {
    ImGui::Begin("Inspector");
    if (selectedNodeId == -1) {
        ImGui::TextDisabled("No node selected.");
        ImGui::End();
        return;
    }

    auto it = tree.nodes.find(selectedNodeId);
    if (it == tree.nodes.end()) {
        ImGui::TextDisabled("Node not found.");
        ImGui::End();
        return;
    }

    DialogueNode& n = it->second;

    // Tipo de nodo
    int typeIndex = (n.type == DialogueNodeType::Basic) ? 0 : 1;
    const char* types[] = { "Basic", "Options" };
    if (ImGui::Combo("Type", &typeIndex, types, IM_ARRAYSIZE(types))) {
        n.type = (typeIndex == 0) ? DialogueNodeType::Basic : DialogueNodeType::Options;
    }

    // Personaje
    static char charBuf[128];
    strncpy_s(charBuf, sizeof(charBuf), n.characterId.c_str(), _TRUNCATE);
    if (ImGui::InputText("Character", charBuf, IM_ARRAYSIZE(charBuf))) {
        n.characterId = charBuf;
    }

    // Texto
    static char textBuf[512];
    strncpy_s(textBuf, sizeof(textBuf), n.text.c_str(), _TRUNCATE);
    if (ImGui::InputTextMultiline("Text", textBuf, IM_ARRAYSIZE(textBuf), ImVec2(0, 100))) {
        n.text = textBuf;
    }

    // Voiceline
    static char voiceBuf[256];
    strncpy_s(voiceBuf, sizeof(voiceBuf), n.voicelineRef.c_str(), _TRUNCATE);
    if (ImGui::InputText("Voiceline", voiceBuf, IM_ARRAYSIZE(voiceBuf))) {
        n.voicelineRef = voiceBuf;
    }

    ImGui::InputInt("Affinity delta", &n.affinityDelta);
    ImGui::InputInt("UI style id", &n.uiStyleId);

    // Hijos
    ImGui::Separator();
    ImGui::Text("Children:");
    for (int child : n.children) {
        ImGui::BulletText("-> %d", child);
        ImGui::SameLine();
        if (ImGui::SmallButton(("Disconnect##" + std::to_string(child)).c_str())) {
            tree.disconnectNodes(n.id, child);
            break;
        }
    }

    // Botón de borrar nodo
    ImGui::Separator();
    if (ImGui::Button("Delete node")) {
        tree.removeNode(n.id);
        selectedNodeId = -1; // deseleccionamos el nodo eliminado
        ImGui::End();
        return; // salimos para evitar acceder al nodo borrado
    }

    ImGui::End();
}
