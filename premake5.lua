include "./vendor/premake/premake_customization/solution_items.lua"

workspace "Atlas"
	architecture "x86_64"
	startproject "Atlas-Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Atlas/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Atlas/vendor/Glad/include"
IncludeDir["imgui"] = "%{wks.location}/Atlas/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Atlas/vendor/ImGuizmo"
IncludeDir["glm"] = "%{wks.location}/Atlas/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Atlas/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Atlas/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Atlas/vendor/yaml-cpp/include"

group "Dependencies"
	include "vendor/premake"
	include "Atlas/vendor/GLFW"
	include "Atlas/vendor/Glad"
	include "Atlas/vendor/imgui"
	include "Atlas/vendor/yaml-cpp"
group ""

include "Atlas"
include "Sandbox"
include "Atlas-Editor"