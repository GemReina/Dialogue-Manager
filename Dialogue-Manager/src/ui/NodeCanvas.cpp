#include "ui/NodeCanvas.h"
#include <imgui.h>
#include <algorithm> // std::clamp
#include <cmath>

static ImVec2 worldToScreen(float x, float y, const CanvasState& s) {
    return ImVec2{ x * s.zoom + s.offsetX, y * s.zoom + s.offsetY };
}

void NodeCanvas::drawGrid() {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImVec2 size = ImGui::GetContentRegionAvail();
    const float gridStep = 32.0f;

    // If content region is zero in either dimension, creating an InvisibleButton with
    // a zero-size will trigger an assertion in ImGui. Create a minimal button to
    // preserve ImGui item behavior (IsItemActive, etc.) used by the canvas interaction.
    if (size.x <= 0.0f || size.y <= 0.0f) {
        ImGui::InvisibleButton("canvas", ImVec2(1.0f, 1.0f));
        return;
    }

    for (float x = origin.x; x < origin.x + size.x; x += gridStep)
        dl->AddLine(ImVec2{ x, origin.y }, ImVec2{ x, origin.y + size.y }, IM_COL32(50, 50, 50, 80));
    for (float y = origin.y; y < origin.y + size.y; y += gridStep)
        dl->AddLine(ImVec2{ origin.x, y }, ImVec2{ origin.x + size.x, y }, IM_COL32(50, 50, 50, 80));
    ImGui::InvisibleButton("canvas", size);
}

void NodeCanvas::drawNode(DialogueNode& node, CanvasState& state, bool isSelected) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 pos = worldToScreen(node.x, node.y, state);
    ImVec2 rectMin = ImVec2{ pos.x, pos.y };
    ImVec2 rectMax = ImVec2{ pos.x + 180 * state.zoom, pos.y + 80 * state.zoom };

    ImU32 bg = isSelected ? IM_COL32(80, 120, 200, 220) : IM_COL32(40, 40, 40, 220);
    ImU32 border = IM_COL32(200, 200, 200, 180);

    dl->AddRectFilled(rectMin, rectMax, bg, 6.0f);
    dl->AddRect(rectMin, rectMax, border, 6.0f);

    // Title
    const char* title = (node.type == DialogueNodeType::Basic ? "Basic" : "Options");
    dl->AddText(ImVec2{ rectMin.x + 8, rectMin.y + 8 }, IM_COL32_WHITE, title);

    // Text preview
    std::string preview = node.characterId.empty() ? "" : node.characterId + ": ";
    preview += node.text.empty() ? "<empty>" : node.text;
    dl->AddText(ImVec2{ rectMin.x + 8, rectMin.y + 28 }, IM_COL32(230, 230, 230, 255), preview.c_str());
}

bool NodeCanvas::nodeRectHit(const DialogueNode& node, float mx, float my, const CanvasState& s) {
    ImVec2 pos = worldToScreen(node.x, node.y, s);
    float w = 180 * s.zoom, h = 80 * s.zoom;
    return mx >= pos.x && mx <= pos.x + w && my >= pos.y && my <= pos.y + h;
}

void NodeCanvas::Draw(DialogueTree& tree, CanvasState& state) {
    ImGui::Begin("Canvas", nullptr, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
    drawGrid();

    ImDrawList* dl = ImGui::GetWindowDrawList();

    // Panning
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
        ImVec2 d = ImGui::GetIO().MouseDelta;
        state.offsetX += d.x;
        state.offsetY += d.y;
    }

    // Zoom
    float wheel = ImGui::GetIO().MouseWheel;
    if (std::fabs(wheel) > 0.001f) {
        float oldZoom = state.zoom;
        state.zoom = std::clamp(state.zoom + wheel * 0.1f, 0.3f, 2.5f);
        // Zoom to cursor pivot
        ImVec2 mouse = ImGui::GetIO().MousePos;
        state.offsetX = mouse.x - (mouse.x - state.offsetX) * (state.zoom / oldZoom);
        state.offsetY = mouse.y - (mouse.y - state.offsetY) * (state.zoom / oldZoom);
    }

    // Draw connections first
    for (const auto& [id, node] : tree.nodes) {
        for (int child : node.children) {
            auto it = tree.nodes.find(child);
            if (it == tree.nodes.end()) continue;
            ImVec2 a = worldToScreen(node.x + 180, node.y + 40, state);
            ImVec2 b = worldToScreen(it->second.x, it->second.y + 40, state);
            dl->AddBezierCubic(a,
                ImVec2{ a.x + 60, a.y },
                ImVec2{ b.x - 60, b.y },
                b,
                IM_COL32(160, 160, 80, 200), 2.0f);
        }
    }

    // Interaction: select/drag/link
    ImVec2 mouse = ImGui::GetIO().MousePos;
    bool clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    int hoverId = -1;
    for (auto& [id, node] : tree.nodes) {
        if (nodeRectHit(node, mouse.x, mouse.y, state)) {
            hoverId = id;
        }
    }

    if (clicked) {
        if (hoverId != -1) {
            state.selectedNodeId = hoverId;
            if (ImGui::GetIO().KeyShift) {
                state.linkFromId = hoverId; // Shift+Click to start linking
            }
            else if (state.linkFromId != -1 && state.linkFromId != hoverId) {
                tree.connectNodes(state.linkFromId, hoverId);
                state.linkFromId = -1;
            }
        }
        else {
            state.selectedNodeId = -1;
        }
    }

    // Drag selected node
    if (state.selectedNodeId != -1 && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        auto& n = tree.nodes[state.selectedNodeId];
        ImVec2 d = ImGui::GetIO().MouseDelta;
        n.x += d.x / state.zoom;
        n.y += d.y / state.zoom;
    }

    // Draw nodes
    for (auto& [id, node] : tree.nodes) {
        drawNode(node, state, id == state.selectedNodeId);
    }

    // Overlay
    ImGui::SetCursorPos(ImVec2{ 10, 10 });
    ImGui::Text("Zoom: %.2f  Offset: (%.1f, %.1f)", state.zoom, state.offsetX, state.offsetY);
    if (state.linkFromId != -1) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Linking from node %d (click target)", state.linkFromId);
    }

    ImGui::End();
}
