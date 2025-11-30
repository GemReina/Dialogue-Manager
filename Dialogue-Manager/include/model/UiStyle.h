#pragma once
#include <string>

// Simple color container used by UiStyle
struct Color {
    float r = 1, g = 1, b = 1, a = 1;
};

// UI style definition used for previewing dialogue text (colors, font, background).
class UiStyle {
public:
    // pre-condition: None
    // post-condition: UiStyle created with default values
    int id = -1;
    std::string name = "Default";
    std::string backgroundImage; // optional
    Color backgroundColor{ 0,0,0,1 };
    std::string fontPath;
    Color textColor{ 1,1,1,1 };
};
