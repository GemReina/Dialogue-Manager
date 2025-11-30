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

    // Show last action for debugging
    if (!tree.lastAction.empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.8f, 0.6f, 1.0f), "Last: %s", tree.lastAction.c_str());
    }

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

    // Prepare list of existing node labels and ids for combos (stable order)
    std::vector<int> nodeIds;
    std::vector<std::string> nodeLabels;
    nodeIds.reserve(tree.nodes.size() + 1);
    nodeLabels.reserve(tree.nodes.size() + 1);
    nodeIds.push_back(-1); // index 0 == none
    nodeLabels.push_back("<none>");
    for (const auto& p : tree.nodes) {
        int nid = p.first;
        const auto& node = p.second;
        nodeIds.push_back(nid);
        nodeLabels.push_back(std::to_string(nid) + ": " + (node.text.empty() ? "<empty>" : node.text));
    }

    // Options handling for Options nodes
    ImGui::Separator();
    if (n.type == DialogueNodeType::Options) {
        ImGui::Text("Options:");
        // List existing options
        for (size_t i = 0; i < n.options.size(); ++i) {
            auto& opt = n.options[i];
            ImGui::PushID((int)i);
            static char optBuf[256];
            strncpy_s(optBuf, sizeof(optBuf), opt.text.c_str(), _TRUNCATE);
            if (ImGui::InputText("Text", optBuf, IM_ARRAYSIZE(optBuf))) {
                opt.text = optBuf;
            }
            ImGui::SameLine();
            ImGui::Text("->");
            ImGui::SameLine();
            ImGui::TextDisabled("Child:");
            ImGui::SameLine();
            ImGui::Text("%d", opt.childId);

            // Inline combo to assign existing node or none
            std::string comboLabel = std::string("Child##opt_") + std::to_string(n.id) + "_" + std::to_string(i);
            int currentIndex = 0; // 0 = none
            if (opt.childId != -1) {
                for (size_t k = 1; k < nodeIds.size(); ++k) {
                    if (nodeIds[k] == opt.childId) { currentIndex = (int)k; break; }
                }
            }
            if (ImGui::BeginCombo(comboLabel.c_str(), nodeLabels[currentIndex].c_str())) {
                for (int k = 0; k < (int)nodeIds.size(); ++k) {
                    if (ImGui::Selectable(nodeLabels[k].c_str())) {
                        int chosenId = nodeIds[k];
                        if (chosenId == -1) {
                            if (opt.childId != -1) {
                                tree.disconnectNodes(n.id, opt.childId);
                                n.children.erase(std::remove(n.children.begin(), n.children.end(), opt.childId), n.children.end());
                                opt.childId = -1;
                                tree.lastAction = "Disconnected option child";
                            }
                        } else {
                            if (opt.childId == chosenId) {
                                // already assigned, do nothing
                                tree.lastAction = "Option already assigned";
                            } else {
                                if (opt.childId != -1) {
                                    tree.disconnectNodes(n.id, opt.childId);
                                    n.children.erase(std::remove(n.children.begin(), n.children.end(), opt.childId), n.children.end());
                                }
                                opt.childId = chosenId;
                                tree.connectNodes(n.id, chosenId);
                                if (std::find(n.children.begin(), n.children.end(), chosenId) == n.children.end()) n.children.push_back(chosenId);
                                auto itc = tree.nodes.find(chosenId);
                                if (itc != tree.nodes.end()) { itc->second.x = n.x + 200.0f; itc->second.y = n.y; }
                                selectedNodeId = chosenId;
                                tree.lastAction = "Assigned option child";
                            }
                        }
                        tree.printTree();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            if (ImGui::SmallButton("Create New Child")) {
                int cid = tree.createNode(DialogueNodeType::Basic);
                if (opt.childId != -1) {
                    tree.disconnectNodes(n.id, opt.childId);
                    n.children.erase(std::remove(n.children.begin(), n.children.end(), opt.childId), n.children.end());
                }
                opt.childId = cid;
                tree.connectNodes(n.id, cid);
                if (std::find(n.children.begin(), n.children.end(), cid) == n.children.end()) n.children.push_back(cid);
                selectedNodeId = cid;
                tree.lastAction = "Created and assigned new option child";
                tree.printTree();
            }

            ImGui::SameLine();
            if (ImGui::SmallButton("Remove")) {
                // if option had a child connection, disconnect it
                if (opt.childId != -1) {
                    tree.disconnectNodes(n.id, opt.childId);
                    n.children.erase(std::remove(n.children.begin(), n.children.end(), opt.childId), n.children.end());
                    tree.lastAction = "Removed option child";
                    tree.printTree();
                }
                n.options.erase(n.options.begin() + i);
                ImGui::PopID();
                break;
            }
            ImGui::PopID();
        }

        if (ImGui::SmallButton("Add Option")) {
            n.options.push_back(Option{});
        }
    }

    // Hijos (legacy)
    ImGui::Separator();
    ImGui::Text("Children:");
    for (int child : n.children) {
        ImGui::BulletText("-> %d", child);
        ImGui::SameLine();
        if (ImGui::SmallButton(("Disconnect##" + std::to_string(child)).c_str())) {
            tree.disconnectNodes(n.id, child);
            // ensure removal from legacy list
            n.children.erase(std::remove(n.children.begin(), n.children.end(), child), n.children.end());
            // clear any option references pointing to this child
            for (auto& opt : n.options) {
                if (opt.childId == child) opt.childId = -1;
            }
            tree.lastAction = "Disconnected legacy child";
            tree.printTree();
            break;
        }
    }

    ImGui::SameLine();
    // Inline combo to assign existing child for legacy section
    {
        std::string comboName = std::string("AssignExistingChild##") + std::to_string(n.id);
        int currIdx = 0;
        // find currIdx if needed (not showing current selection here)
        if (ImGui::BeginCombo(comboName.c_str(), nodeLabels[currIdx].c_str())) {
            for (int k = 0; k < (int)nodeIds.size(); ++k) {
                if (ImGui::Selectable(nodeLabels[k].c_str())) {
                    int chosenId = nodeIds[k];
                    if (chosenId != -1) {
                        tree.connectNodes(n.id, chosenId);
                        if (std::find(n.children.begin(), n.children.end(), chosenId) == n.children.end()) n.children.push_back(chosenId);
                        auto itc = tree.nodes.find(chosenId);
                        if (itc != tree.nodes.end()) { itc->second.x = n.x + 200.0f; itc->second.y = n.y; }
                        selectedNodeId = chosenId;
                        tree.lastAction = "Assigned legacy child";
                        tree.printTree();
                    }
                }
            }
            ImGui::EndCombo();
        }
    }

    ImGui::SameLine();
    // Add child button: create a new child node and link it
    if (ImGui::SmallButton("Add Child")) {
        int cid = tree.createNode(DialogueNodeType::Basic);
        tree.connectNodes(n.id, cid);
        if (std::find(n.children.begin(), n.children.end(), cid) == n.children.end()) n.children.push_back(cid);
        selectedNodeId = cid; // select the newly created child
        tree.lastAction = "Added new child";
        tree.printTree();
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
