#pragma once
#include <string>

class Character {
public:
    std::string id;
    std::string displayName;
    int affinityBase = 0;
    std::string portraitRef;
    int defaultUiStyleId = -1;
};
