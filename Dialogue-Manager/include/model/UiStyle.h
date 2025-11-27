#pragma once
#include <string>

struct Color {
    float r = 1, g = 1, b = 1, a = 1;
};

class UiStyle {
public:
    int id = -1;
    std::string name = "Default";
    std::string backgroundImage; // optional
    Color backgroundColor{ 0,0,0,1 };
    std::string fontPath;
    Color textColor{ 1,1,1,1 };
};
