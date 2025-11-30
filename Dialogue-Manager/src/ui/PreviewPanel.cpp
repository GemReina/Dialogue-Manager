#include "ui/PreviewPanel.h"
#include <imgui.h>
#include "runtime/DialoguePlayer.h"

void PreviewPanel::Draw(DialogueTree& tree, int& selectedNodeId) {
    ImGui::Begin("Preview");

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

    const DialogueNode& node = it->second;

    // Simple preview: character + text
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "%s", node.characterId.c_str());
    ImGui::Separator();
    ImGui::TextWrapped("%s", node.text.c_str());

    if (node.type == DialogueNodeType::Options) {
        ImGui::Separator();
        ImGui::Text("Options:");
        for (size_t i = 0; i < node.options.size(); ++i) {
            const auto& opt = node.options[i];
            if (ImGui::Button(opt.text.empty() ? ("Option " + std::to_string(i)).c_str() : opt.text.c_str())) {
                if (opt.childId != -1 && tree.nodes.count(opt.childId)) {
                    selectedNodeId = opt.childId;
                }
            }
        }
    } else {
        // Basic node: if it has a single legacy child, allow Next
        if (!node.children.empty()) {
            if (ImGui::Button("Next")) {
                int cid = node.children.front();
                if (tree.nodes.count(cid)) selectedNodeId = cid;
            }
        }
    }

    ImGui::End();
}

void PreviewPanel::DrawRuntime(DialoguePlayer& player) {
    ImGui::Begin("Runtime Preview");
    const DialogueNode* node = player.GetCurrentNode();
    if (!node) {
        ImGui::TextDisabled("Not playing");
        if (ImGui::Button("Start at root")) {
            // can't start here because player needs tree; leave control to user code
        }
        ImGui::End();
        return;
    }

    ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "%s", node->characterId.c_str());
    ImGui::Separator();
    ImGui::TextWrapped("%s", node->text.c_str());

    auto opts = player.GetOptions();
    if (!opts.empty()) {
        ImGui::Separator();
        ImGui::Text("Options:");
        for (size_t i = 0; i < opts.size(); ++i) {
            auto &p = opts[i];
            if (ImGui::Button(p.first.c_str())) {
                player.ChooseOption(i);
            }
        }
    } else {
        if (ImGui::Button("Next")) player.Next();
    }

    ImGui::End();
}
