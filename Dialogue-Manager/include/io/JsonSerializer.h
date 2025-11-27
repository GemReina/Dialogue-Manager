#pragma once
#include "../model/DialogueTree.h"

namespace io {

    class JsonSerializer {
    public:
        static bool Save(const DialogueTree& tree, const std::string& path);
        static bool Load(DialogueTree& tree, const std::string& path);
    };

} // namespace io
