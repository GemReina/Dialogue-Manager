project "Dialogue-Manager"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("%{wks.location}/binaries/bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("%{wks.location}/binaries/bin-int/" .. OutputDir .. "/%{prj.name}")

    files { "include/**.h", "src/**.cpp" }

    includedirs
    {
        "src",
        "include",
        "%{IncludeDir.SFML}",
        "%{IncludeDir.ImGui}",
    }

    libdirs
    {
        "%{LibDir.SFML}",
    }

    links {
        "opengl32.lib",
        "ImGui"
    }

    postbuildcommands
    {
        "for /r \"%{BinDir.SFML}\" %%f in (*.dll) do copy \"%%f\" \"../binaries/bin/" .. OutputDir .. "/Dialogue-Manager\"",
    }

    filter "system:windows"
        systemversion "latest"
        defines { "WINDOWS" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"
        links
        {
            "sfml-system-d.lib",
            "sfml-graphics-d.lib",
            "sfml-window-d.lib",
        }

    filter "configurations:Release"
        defines { "RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"
        links
        {
            "sfml-system.lib",
            "sfml-graphics.lib",
            "sfml-window.lib",
        }
