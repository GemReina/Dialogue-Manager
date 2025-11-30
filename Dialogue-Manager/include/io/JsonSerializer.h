#pragma once
#include "../model/DialogueTree.h"

namespace io {

    // JSON serializer for DialogueTree - saves and loads the entire tree structure.
    class JsonSerializer {
    public:
        // pre-condition: tree is populated and path is writable
        // post-condition: writes JSON representation of tree to path; returns true on success
        static bool Save(const DialogueTree& tree, const std::string& path);
        // pre-condition: file at path exists and contains valid JSON representation
        // post-condition: loads tree from JSON, returns true on success
        static bool Load(DialogueTree& tree, const std::string& path);
    };

} // namespace io
