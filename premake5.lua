include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

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

group "Dependencies"
	include "vendor/premake"
	include "Atlas/vendor/GLFW"
	include "Atlas/vendor/Glad"
	include "Atlas/vendor/imgui"
	include "Atlas/vendor/yaml-cpp"
	include "Atlas/vendor/assimp"
group ""

include "Atlas"
include "Atlas-Editor"