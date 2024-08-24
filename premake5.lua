workspace "Atlas"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Atlas/vendor/GLFW/include"
IncludeDir["Glad"] = "Atlas/vendor/Glad/include"
IncludeDir["imgui"] = "Atlas/vendor/imgui"
IncludeDir["glm"] = "Atlas/vendor/glm"
IncludeDir["stb_image"] = "Atlas/vendor/stb_image"

group "Dependencies"
	include "Atlas/vendor/GLFW"
	include "Atlas/vendor/Glad"
	include "Atlas/vendor/imgui"
group ""

project "Atlas"
	location "Atlas"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "atlaspch.h"
	pchsource "Atlas/src/atlaspch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}

	links
	{
		"GLFW",
		"Glad",
		"imgui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ATLAS_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "ATLAS_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ATLAS_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ATLAS_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"Atlas/vendor/spdlog/include",
		"Atlas/src",
		"Atlas/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Atlas"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "ATLAS_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ATLAS_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ATLAS_DIST"
		runtime "Release"
		optimize "on"
