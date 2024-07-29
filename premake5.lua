workspace "Atlas"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Atlas/vendor/GLFW/include"
IncludeDir["Glad"] = "Atlas/vendor/Glad/include"
IncludeDir["imgui"] = "Atlas/vendor/imgui"
IncludeDir["glm"] = "Atlas/vendor/glm"

include "Atlas/vendor/GLFW"
include "Atlas/vendor/Glad"
include "Atlas/vendor/imgui"

project "Atlas"
	location "Atlas"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "atlaspch.h"
	pchsource "Atlas/src/atlaspch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"GLFW",
		"Glad",
		"imgui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"ATLAS_PLATFORM_WINDOWS",
			"ATLAS_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "ATLAS_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "ATLAS_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "ATLAS_DIST"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

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
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"ATLAS_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "ATLAS_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "ATLAS_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "ATLAS_DIST"
		runtime "Release"
		optimize "On"