// Main entry for the Dialogue Editor application.
// pre-condition: SFML and ImGui-SFML libraries are available and linked.
// post-condition: Initializes UI, runs main editor loop until window close, returns 0 on success.

#define _CRT_SECURE_NO_WARNINGS
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "ui/EditorApp.h"

// Set colors used for the dark, cozy theme.
// pre-condition: ImGui context has been created (ImGui::CreateContext called)
// post-condition: ImGui style colors and metrics are modified to a warm, cozy dark palette
static void SetDarkThemeColors() {
    ImGuiStyle& style = ImGui::GetStyle();
    auto& colors = style.Colors;

    // Cozy dark background (slightly warm dark gray)
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.08f, 0.07f, 0.09f, 1.0f };

    // Panels / frames: slightly lighter than background
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.14f, 0.12f, 0.15f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.18f, 0.16f, 0.20f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.12f, 0.10f, 0.13f, 1.0f };

    // Headers use warm brown tone
    colors[ImGuiCol_Header] = ImVec4{ 0.36f, 0.28f, 0.24f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.45f, 0.34f, 0.30f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.30f, 0.24f, 0.20f, 1.0f };

    // Buttons: soft peach accent
    colors[ImGuiCol_Button] = ImVec4{ 0.92f, 0.72f, 0.63f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.98f, 0.82f, 0.74f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.82f, 0.62f, 0.54f, 1.0f };

    // Tabs: subtle warm surfaces
    colors[ImGuiCol_Tab] = ImVec4{ 0.12f, 0.10f, 0.13f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.20f, 0.16f, 0.18f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.22f, 0.20f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.10f, 0.09f, 0.11f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.14f, 0.12f, 0.14f, 1.0f };

    // Title bar
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.10f, 0.09f, 0.11f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.12f, 0.10f, 0.13f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.08f, 0.07f, 0.09f, 1.0f };

    // Scrollbars and grips - muted neutrals
    colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.88f, 0.85f, 0.82f, 0.12f };
    colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.88f, 0.85f, 0.82f, 0.22f };
    colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.88f, 0.85f, 0.82f, 0.40f };
    colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.06f, 0.05f, 0.06f, 0.3f };
    colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.38f, 0.36f, 0.35f, 1.0f };
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.48f, 0.46f, 0.45f, 1.0f };
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.56f, 0.53f, 0.52f, 1.0f };

    // Checkmark and slider accents: pastel teal for cozy highlight
    colors[ImGuiCol_CheckMark] = ImVec4{ 0.58f, 0.82f, 0.78f, 1.0f };
    colors[ImGuiCol_SliderGrab] = ImVec4{ 0.70f, 0.85f, 0.81f, 0.9f };
    colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.88f, 0.95f, 0.92f, 1.0f };

    // Text colors: warm off-white for readability
    colors[ImGuiCol_Text] = ImVec4{ 0.98f, 0.95f, 0.90f, 1.0f };
    colors[ImGuiCol_TextDisabled] = ImVec4{ 0.78f, 0.74f, 0.70f, 1.0f };

    // Option line colors
    colors[ImGuiCol_Border] = ImVec4{ 0.16f, 0.14f, 0.16f, 0.6f };

    // Make things soft and cozy: rounded corners, comfortable paddings
    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.PopupRounding = 6.0f;

    style.WindowPadding = ImVec2(12, 10);
    style.FramePadding = ImVec2(10, 6);
    style.ItemSpacing = ImVec2(8, 6);
    style.ScrollbarSize = 14.0f;

    // Slight border for separated panels
    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 1.0f;
}

// Enable docking and keyboard navigation flags for ImGui.
// pre-condition: ImGui context created
// post-condition: ImGuiIO.ConfigFlags updated to enable docking and keyboard navigation
static void EnableDocking() {
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

// Application entry point
// pre-condition: SFML and ImGui-SFML are properly linked; windowing system available
// post-condition: Runs UI main loop until window closed; returns 0 on success or -1 on initialization failure
int main() {
    auto desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Dialogue Editor", sf::Style::Default);
    window.setFramerateLimit(60);
    if (!ImGui::SFML::Init(window)) {
        std::cerr << "Failed to init ImGui-SFML\n";
        return -1;
    }
    EnableDocking();
    ImGui::StyleColorsDark();
    SetDarkThemeColors();

    EditorApp app;

    sf::Clock deltaClock;
    bool running = true;
    while (running) {
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>()) running = false;
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        app.Frame();

        window.clear(sf::Color(25, 25, 25));
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
