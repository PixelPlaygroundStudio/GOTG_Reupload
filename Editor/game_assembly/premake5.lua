workspace "WiwaGameAssembly"
	architecture "x86_64"
	startproject "WiwaGameAssembly"
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

IncludeDirs = {}
IncludeDirs["Wiwa"] = "%{wks.location}/../../Wiwa/src"
IncludeDirs["GLFW"] = "%{wks.location}/../../Wiwa/vendor/GLFW/include"
IncludeDirs["filewatch"] = "%{wks.location}/../../Wiwa/vendor/filewatch"
IncludeDirs["Glew"] = "%{wks.location}/../../Wiwa/vendor/Glew/include/GL"
IncludeDirs["ImGui"] = "%{wks.location}/../../Wiwa/vendor/imgui"
IncludeDirs["GLM"] = "%{wks.location}/../../Wiwa/vendor/glm"
IncludeDirs["PCG"] = "%{wks.location}/../../Wiwa/vendor/PCG/include"
IncludeDirs["JSON"] = "%{wks.location}/../../Wiwa/vendor/rapidjson"
IncludeDirs["XML"] = "%{wks.location}/../../Wiwa/vendor/pugixml/src"
IncludeDirs["mono"] = "%{wks.location}/../../Wiwa/vendor/mono/include"
IncludeDirs["SPDLOG"] = "%{wks.location}/../../Wiwa/vendor/spdlog/include"
IncludeDirs["Optick"] = "%{wks.location}/../../Wiwa/vendor/Optick/include"
IncludeDirs["assimp"] = "%{wks.location}/../../Wiwa/vendor/assimp/include"
IncludeDirs["ImGuizmo"] = "%{wks.location}/../../Wiwa/vendor/imguizmo"
IncludeDirs["GLI"] = "%{wks.location}/../../Wiwa/vendor/gli/gli"
IncludeDirs["Wwise"] = "%{wks.location}/../../Wiwa/vendor/Wwise/include"
IncludeDirs["bullet"] = "%{wks.location}/../../Wiwa/vendor/bullet/include"
IncludeDirs["OpenCV"] = "%{wks.location}/../../Wiwa/vendor/opencv-4.7.0/include"
IncludeDirs["recastnavigation"] = "%{wks.location}/../../Wiwa/vendor/recastnavigation/include"
IncludeDirs["ozz"] = "%{wks.location}/../../Wiwa/vendor/ozz/include"

project "WiwaGameAssembly"
	kind "SharedLib"
	language "C++"

	targetdir ("%{wks.location}/bin/%{prj.name}")
	objdir ("%{wks.location}/bin-int/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"../assets/**.h",
		"../assets/**.cpp"
	}

	includedirs
	{
		"%{IncludeDirs.SPDLOG}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.GLM}",
		"%{IncludeDirs.Wiwa}",
		"%{IncludeDirs.Optick}",
		"%{IncludeDirs.ImGuizmo}",
		"%{IncludeDirs.filewatch}",
		"%{IncludeDirs.Wwise}",
		"%{IncludeDirs.bullet}",
		"%{IncludeDirs.OpenCV}",
		"%{IncludeDirs.recastnavigation}",
		"%{IncludeDirs.ozz}"
	}

	links
	{
		"Wiwa",
		"ImGui"
	}

	debugdir ".."
	debugcommand("../WiwaEditor.exe")


	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"WI_PLATFORM_WINDOWS"
		}
		postbuildcommands
		{
			("{COPY} \"%{cfg.buildtarget.relpath}\" \"%{wks.location}build\"")
		}

	filter "configurations:Debug"
		defines "WI_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "WI_RELEASE"
		runtime "Release"

		symbols "On"
		optimize "On"

	filter "configurations:Dist"
		defines "WI_DIST"
		runtime "Release"
		symbols "Off"
		optimize "On"