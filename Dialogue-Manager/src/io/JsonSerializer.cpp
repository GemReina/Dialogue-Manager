#include "io/JsonSerializer.h"
#include <external/nlohmann/json.hpp>
#include <fstream>

using nlohmann::json;

namespace {

    json toJson(const DialogueTree& t) {
        json j;
        j["version"] = 1;
        j["name"] = t.name;
        j["rootId"] = t.rootId;
        j["nextNodeId"] = t.nextNodeId;

        j["nodes"] = json::array();
        for (const auto& [id, n] : t.nodes) {
            json jn;
            jn["id"] = n.id;
            jn["type"] = (n.type == DialogueNodeType::Basic ? "Basic" : "Options");
            jn["characterId"] = n.characterId;
            jn["text"] = n.text;
            jn["voicelineRef"] = n.voicelineRef;
            jn["affinityDelta"] = n.affinityDelta;
            jn["uiStyleId"] = n.uiStyleId;
            jn["pos"] = { n.x, n.y };

            jn["children"] = n.children;

            // conditions
            jn["conditions"] = json::array();
            for (const auto& c : n.conditions) {
                jn["conditions"].push_back({ {"key", c.key}, {"op", c.op}, {"value", c.value} });
            }

            // events
            jn["events"] = json::array();
            for (const auto& e : n.events) {
                json je;
                je["name"] = e.name;
                je["params"] = e.params;
                jn["events"].push_back(je);
            }

            j["nodes"].push_back(jn);
        }

        // characters
        j["characters"] = json::array();
        for (const auto& [cid, c] : t.characters) {
            j["characters"].push_back({
                {"id", c.id},
                {"displayName", c.displayName},
                {"affinityBase", c.affinityBase},
                {"portraitRef", c.portraitRef},
                {"defaultUiStyleId", c.defaultUiStyleId}
                });
        }

        // styles
        j["styles"] = json::array();
        for (const auto& [sid, s] : t.styles) {
            j["styles"].push_back({
                {"id", s.id},
                {"name", s.name},
                {"backgroundImage", s.backgroundImage},
                {"backgroundColor", {s.backgroundColor.r, s.backgroundColor.g, s.backgroundColor.b, s.backgroundColor.a}},
                {"fontPath", s.fontPath},
                {"textColor", {s.textColor.r, s.textColor.g, s.textColor.b, s.textColor.a}}
                });
        }

        return j;
    }

    void fromJson(DialogueTree& t, const json& j) {
        t.name = j.value("name", "Unnamed");
        t.rootId = j.value("rootId", -1);
        t.nextNodeId = j.value("nextNodeId", 1);

        t.nodes.clear();
        for (const auto& jn : j.at("nodes")) {
            DialogueNode n;
            n.id = jn.at("id").get<int>();
            std::string type = jn.at("type").get<std::string>();
            n.type = (type == "Options") ? DialogueNodeType::Options : DialogueNodeType::Basic;
            n.characterId = jn.value("characterId", "");
            n.text = jn.value("text", "");
            n.voicelineRef = jn.value("voicelineRef", "");
            n.affinityDelta = jn.value("affinityDelta", 0);
            n.uiStyleId = jn.value("uiStyleId", -1);

            auto pos = jn.value("pos", std::vector<float>{0.0f, 0.0f});
            if (pos.size() >= 2) { n.x = pos[0]; n.y = pos[1]; }

            n.children = jn.value("children", std::vector<int>{});

            n.conditions.clear();
            if (jn.contains("conditions")) {
                for (const auto& jc : jn["conditions"]) {
                    Condition c;
                    c.key = jc.value("key", "");
                    c.op = jc.value("op", "");
                    c.value = jc.value("value", "");
                    n.conditions.push_back(c);
                }
            }

            n.events.clear();
            if (jn.contains("events")) {
                for (const auto& je : jn["events"]) {
                    Event e;
                    e.name = je.value("name", "");
                    if (je.contains("params")) {
                        e.params = je["params"].get<std::unordered_map<std::string, std::string>>();
                    }
                    n.events.push_back(e);
                }
            }

            t.nodes.emplace(n.id, std::move(n));
        }

        t.characters.clear();
        if (j.contains("characters")) {
            for (const auto& jc : j["characters"]) {
                Character c;
                c.id = jc.value("id", "");
                c.displayName = jc.value("displayName", "");
                c.affinityBase = jc.value("affinityBase", 0);
                c.portraitRef = jc.value("portraitRef", "");
                c.defaultUiStyleId = jc.value("defaultUiStyleId", -1);
                t.characters.emplace(c.id, std::move(c));
            }
        }

        t.styles.clear();
        if (j.contains("styles")) {
            for (const auto& js : j["styles"]) {
                UiStyle s;
                s.id = js.value("id", -1);
                s.name = js.value("name", "Default");
                s.backgroundImage = js.value("backgroundImage", "");
                auto bg = js.value("backgroundColor", std::vector<float>{0, 0, 0, 1});
                if (bg.size() >= 4) { s.backgroundColor = { bg[0], bg[1], bg[2], bg[3] }; }
                s.fontPath = js.value("fontPath", "");
                auto tc = js.value("textColor", std::vector<float>{1, 1, 1, 1});
                if (tc.size() >= 4) { s.textColor = { tc[0], tc[1], tc[2], tc[3] }; }
                t.styles.emplace(s.id, std::move(s));
            }
        }
    }

} // anon

namespace io {

    bool JsonSerializer::Save(const DialogueTree& tree, const std::string& path) {
        std::ofstream out(path, std::ios::binary);
        if (!out) return false;
        json j = toJson(tree);
        out << j.dump(2);
        return true;
    }

    bool JsonSerializer::Load(DialogueTree& tree, const std::string& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in) return false;
        json j;
        in >> j;
        fromJson(tree, j);
        return true;
    }

} // namespace io
