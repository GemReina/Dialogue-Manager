include "Dependencies.lua"

workspace "Dialogue-Manager"
   architecture "x64"
   configurations { "Debug", "Release" }
   startproject "Dialogue-Manager"

   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


group "Dependencies"
	include "Dialogue-Manager/vendor/ImGui"
group ""

include "Dialogue-Manager/Build-App.lua"