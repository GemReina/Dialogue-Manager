#define _CRT_SECURE_NO_WARNINGS
#include "ui/NodeCanvas.h"
#include <imgui.h>
#include <algorithm>
#include <cmath>
#include <set>

static ImVec2 worldToScreen(float x, float y, const CanvasState& s) {
    return ImVec2{ x * s.zoom + s.offsetX, y * s.zoom + s.offsetY };
}

void NodeCanvas::drawGrid() {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImVec2 size = ImGui::GetContentRegionAvail();
    const float gridStep = 32.0f;

    // Avoid passing zero-size to InvisibleButton (would assert in ImGui debug builds)
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

    const char* title = (node.type == DialogueNodeType::Basic ? "Basic" : "Options");
    dl->AddText(ImVec2{ rectMin.x + 8, rectMin.y + 8 }, IM_COL32_WHITE, title);

    std::string preview = node.characterId.empty() ? "" : node.characterId + ": ";
    preview += node.text.empty() ? "<empty>" : node.text;
    dl->AddText(ImVec2{ rectMin.x + 8, rectMin.y + 28 }, IM_COL32(230, 230, 230, 255), preview.c_str());

    // If Options node, render option texts inside node box
    if (node.type == DialogueNodeType::Options && !node.options.empty()) {
        float startY = rectMin.y + 28.0f;
        for (size_t i = 0; i < node.options.size(); ++i) {
            const auto& opt = node.options[i];
            dl->AddText(ImVec2{ rectMin.x + 8, startY + i * 16.0f }, IM_COL32(200, 200, 120, 255), opt.text.c_str());
        }
    }

    // Debug: render child IDs and option child IDs inside node for diagnostics
    std::string childLine = "C:";
    for (int c : node.children) childLine += " " + std::to_string(c);
    dl->AddText(ImVec2{ rectMin.x + 8, rectMax.y - 22 }, IM_COL32(180, 180, 180, 200), childLine.c_str());

    if (!node.options.empty()) {
        std::string optLine = "O:";
        for (const auto& o : node.options) optLine += " " + std::to_string(o.childId);
        dl->AddText(ImVec2{ rectMin.x + 8, rectMax.y - 12 }, IM_COL32(180, 160, 120, 200), optLine.c_str());
    }
}

bool NodeCanvas::nodeRectHit(const DialogueNode& node, float mx, float my, const CanvasState& s) {
    ImVec2 pos = worldToScreen(node.x, node.y, s);
    float w = 180 * s.zoom, h = 80 * s.zoom;
    // If options node, expand hit area slightly to include options text
    return mx >= pos.x && mx <= pos.x + w && my >= pos.y && my <= pos.y + h;
}

void NodeCanvas::Draw(DialogueTree& tree, CanvasState& state) {
    ImGui::Begin("Canvas", nullptr, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

    // Capture canvas rect before drawGrid() (drawGrid will add the InvisibleButton that
    // becomes the last item). This origin/size represent the interactive canvas area.
    ImVec2 canvasOrigin = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    drawGrid();

    ImDrawList* dl = ImGui::GetWindowDrawList();

    ImVec2 mouse = ImGui::GetIO().MousePos;
    bool mouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    bool mouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    bool mouseReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Left);

    bool inCanvas = (mouse.x >= canvasOrigin.x && mouse.x <= canvasOrigin.x + canvasSize.x &&
                     mouse.y >= canvasOrigin.y && mouse.y <= canvasOrigin.y + canvasSize.y);

    // Determine which node is under mouse (screen space)
    int hoverId = -1;
    for (auto& [id, node] : tree.nodes) {
        if (nodeRectHit(node, mouse.x, mouse.y, state)) {
            hoverId = id;
            break;
        }
    }

    // Start panning if user clicked on background (no node) inside canvas
    if (mouseClicked && inCanvas && hoverId == -1) {
        state.panning = true;
        state.panStartMouseX = mouse.x;
        state.panStartMouseY = mouse.y;
        state.panStartOffsetX = state.offsetX;
        state.panStartOffsetY = state.offsetY;
        // clear selection when clicking background
        state.selectedNodeId = -1;
    }

    // Update panning while left mouse is held
    if (state.panning) {
        if (mouseDown) {
            float dx = mouse.x - state.panStartMouseX;
            float dy = mouse.y - state.panStartMouseY;
            state.offsetX = state.panStartOffsetX + dx;
            state.offsetY = state.panStartOffsetY + dy;
        }
        if (mouseReleased) {
            state.panning = false;
        }
    }

    // Middle mouse drag also pans (preserve existing behavior)
    if (!state.panning && ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
        ImVec2 d = ImGui::GetIO().MouseDelta;
        state.offsetX += d.x;
        state.offsetY += d.y;
    }

    // Zoom with mouse wheel
    float wheel = ImGui::GetIO().MouseWheel;
    if (std::fabs(wheel) > 0.001f) {
        float oldZoom = state.zoom;
        state.zoom = std::clamp(state.zoom + wheel * 0.1f, 0.3f, 2.5f);
        ImVec2 m = ImGui::GetIO().MousePos;
        state.offsetX = m.x - (m.x - state.offsetX) * (state.zoom / oldZoom);
        state.offsetY = m.y - (m.y - state.offsetY) * (state.zoom / oldZoom);
    }

    // Track drawn edges to avoid duplicates (option child + legacy child)
    std::set<std::pair<int,int>> drawn;

    // Draw connections (legacy children)
    for (const auto& [id, node] : tree.nodes) {
        for (int child : node.children) {
            if (drawn.find({id, child}) != drawn.end()) continue;
            auto it = tree.nodes.find(child);
            if (it == tree.nodes.end()) continue;
            ImVec2 a = worldToScreen(node.x + 180, node.y + 40, state);
            ImVec2 b = worldToScreen(it->second.x, it->second.y + 40, state);
            dl->AddBezierCubic(a,
                ImVec2{ a.x + 60, a.y },
                ImVec2{ b.x - 60, b.y },
                b,
                IM_COL32(160, 160, 80, 200), 2.0f);
            drawn.insert({id, child});
        }

        // Additionally draw options-based children for Options nodes
        if (node.type == DialogueNodeType::Options) {
            for (size_t i = 0; i < node.options.size(); ++i) {
                int cid = node.options[i].childId;
                if (cid == -1) continue;
                if (drawn.find({id, cid}) != drawn.end()) continue; // skip duplicate
                auto it = tree.nodes.find(cid);
                if (it == tree.nodes.end()) continue;
                // compute anchor point along right edge based on option index
                float localY_world = node.y + (28.0f / state.zoom) + float(i) * (16.0f / state.zoom); // convert screen px to world units
                ImVec2 a = worldToScreen(node.x + 180, localY_world, state);
                ImVec2 b = worldToScreen(it->second.x, it->second.y + 40, state);
                dl->AddBezierCubic(a,
                    ImVec2{ a.x + 60, a.y },
                    ImVec2{ b.x - 60, b.y },
                    b,
                    IM_COL32(200, 160, 120, 200), 2.0f);
                drawn.insert({id, cid});
            }
        }
    }

    // Interaction: select/link when user clicked on canvas and clicked over a node
    if (mouseClicked && inCanvas && !state.panning) {
        if (hoverId != -1) {
            if (ImGui::GetIO().KeyShift) {
                state.linkFromId = hoverId;
            }
            else if (state.linkFromId != -1 && state.linkFromId != hoverId) {
                tree.connectNodes(state.linkFromId, hoverId);
                state.linkFromId = -1;
            }
            else {
                state.selectedNodeId = hoverId;
            }
        }
        else {
            state.selectedNodeId = -1;
        }
    }

    // Drag selected node (only if not panning)
    if (!state.panning && state.selectedNodeId != -1 && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
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
