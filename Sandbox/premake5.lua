project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Atlas/vendor/spdlog/include",
		"%{wks.location}/Atlas/src",
		"%{wks.location}/Atlas/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
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