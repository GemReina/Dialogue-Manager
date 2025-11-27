#define _CRT_SECURE_NO_WARNINGS
#include "ui/CharacterPanel.h"
#include <imgui.h>
#include <cstring>

void CharacterPanel::Draw(DialogueTree& tree) {
    ImGui::Begin("Characters");

    static char newId[64] = "";
    static char newName[128] = "";
    ImGui::InputText("New id", newId, IM_ARRAYSIZE(newId));
    ImGui::InputText("Display name", newName, IM_ARRAYSIZE(newName));
    if (ImGui::Button("Add character")) {
        Character c;
        c.id = newId;
        c.displayName = newName;
        tree.characters.emplace(c.id, c);
        newId[0] = '\0';
        newName[0] = '\0';
    }

    ImGui::Separator();
    for (auto it = tree.characters.begin(); it != tree.characters.end();) {
        Character& c = it->second;
        ImGui::PushID(c.id.c_str());

        static char idBuf[64];
        strncpy_s(idBuf, sizeof(idBuf), c.id.c_str(), _TRUNCATE);
        if (ImGui::InputText("Id", idBuf, IM_ARRAYSIZE(idBuf))) {
            std::string newIdStr = idBuf;
            if (!newIdStr.empty() && newIdStr != c.id) {
                Character moved = c;
                moved.id = newIdStr;
                it = tree.characters.erase(it);
                tree.characters.emplace(moved.id, moved);
                ImGui::PopID();
                continue;
            }
        }

        static char nameBuf[128];
        strncpy_s(nameBuf, sizeof(nameBuf), c.displayName.c_str(), _TRUNCATE);
        if (ImGui::InputText("Display name", nameBuf, IM_ARRAYSIZE(nameBuf))) {
            c.displayName = nameBuf;
        }

        ImGui::InputInt("Affinity base", &c.affinityBase);
        ImGui::InputInt("Default style id", &c.defaultUiStyleId);

        if (ImGui::Button("Remove")) {
            it = tree.characters.erase(it);
            ImGui::PopID();
            continue;
        }

        ImGui::Separator();
        ImGui::PopID();
        ++it;
    }

    ImGui::End();
}
