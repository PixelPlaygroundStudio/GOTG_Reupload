project "Wiwa"
	kind "SharedLib"
	language "C++"

	
	targetdir ("%{wks.location}/bin/".. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/".. outputdir .. "/%{prj.name}")

	pchheader "wipch.h"
	pchsource "src/wipch.cpp"

	ignoredefaultlibraries 
	{
		"libcmtd"
	}
	
	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"src",
		"%{IncludeDirs.SPDLOG}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.filewatch}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.GLM}",
		"%{IncludeDirs.Glew}",
		"%{IncludeDirs.PCG}",
		"%{IncludeDirs.JSON}",
		"%{IncludeDirs.NJSON}",
		"%{IncludeDirs.XML}",
		"%{IncludeDirs.Optick}",
		"%{IncludeDirs.assimp}",
		"%{IncludeDirs.ImGuizmo}",
		"%{IncludeDirs.Wwise}",
		"%{IncludeDirs.IconFontCppHeaders}",
		"%{IncludeDirs.bullet}",
		"%{IncludeDirs.ozz}",
		"%{IncludeDirs.recastnavigation}",
		"%{IncludeDirs.OpenCV}"
	}

	links
	{
		"GLFW",
		"ImGui",
		"opengl32.lib",
		"%{wks.location}/Wiwa/vendor/Glew/lib/glew32.lib",
		"Optick",
		"ImGuizmo",
		"ozz",
		"%{wks.location}/Wiwa/vendor/assimp/lib/x64/release/assimp-vc143-mt.lib",
		"%{Library.Wwise_SE}",
		"%{Library.Wwise_MMgr}",
		"%{Library.Wwise_SMgr}",
		"%{Library.Wwise_ME}",
		"%{Library.Wwise_SA}",
		"%{Library.Wwise_CC}"
	}

	filter "files:vendor/imguizmo/**.cpp"
		flags {"NoPCH"}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"
		defines
		{
			"WI_PLATFORM_WINDOWS",
			"WI_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
			"_CRT_SECURE_NO_WARNINGS"
		}


		postbuildcommands
		{
			"{COPY} \"%{cfg.buildtarget.relpath}\" \"%{wks.location}Editor\"",
			"copy \"..\\bin\\%{outputdir}\\Wiwa\\Wiwa.lib\" \"%{wks.location}Editor\\game\\Wiwa.lib\"",
			"copy \"..\\bin\\%{outputdir}\\Wiwa\\Wiwa.lib\" \"%{wks.location}Editor\\game_assembly\\Wiwa.lib\""
		}

	debugdir "%{wks.location}/Editor"
	
	filter "configurations:Debug"
		defines
		{ 
			"WI_DEBUG",
			"WI_ENABLE_ASSERTS"
		}
		runtime "Debug"
		symbols "On"
		links
		{
			"%{Library.bullet_dynamics_debug}",
			"%{Library.bullet_collision_debug}",
			"%{Library.bullet_linear_math_debug}",
			"%{Library.DebugUtils_debug}",
			"%{Library.Detour_debug}",
			"%{Library.DetourCrowd_debug}",
			"%{Library.DetourTileCache_debug}",
			"%{Library.Recast_debug}",
			"%{Library.opencv_core_debug}",
			"%{Library.opencv_video_debug}",
			"%{Library.opencv_videoio_debug}",
			"%{Library.opencv_highgui_debug}",
			"%{Library.opencv_imgproc_debug}",
			"%{Library.opencv_imgcodecs_debug}"
		}


	filter "configurations:Release"
		defines "WI_RELEASE"
		runtime "Release"
		optimize "On"
		symbols "On"
		links
		{
			"%{Library.bullet_dynamics_release}",
			"%{Library.bullet_collision_release}",
			"%{Library.bullet_linear_math_release}",
			"%{Library.DebugUtils_release}",
			"%{Library.Detour_release}",
			"%{Library.DetourCrowd_release}",
			"%{Library.DetourTileCache_release}",
			"%{Library.Recast_release}",
			"%{Library.opencv_core}",
			"%{Library.opencv_video}",
			"%{Library.opencv_videoio}",
			"%{Library.opencv_highgui}",
			"%{Library.opencv_imgproc}",
			"%{Library.opencv_imgcodecs}"
		}

	filter "configurations:Dist"
		defines "WI_DIST"
		runtime "Release"
		optimize "On"
		symbols "Off"
		links
		{
			"%{Library.bullet_dynamics_release}",
			"%{Library.bullet_collision_release}",
			"%{Library.bullet_linear_math_release}",
			"%{Library.DebugUtils_release}",
			"%{Library.Detour_release}",
			"%{Library.DetourCrowd_release}",
			"%{Library.DetourTileCache_release}",
			"%{Library.Recast_release}",
			"%{Library.opencv_core}",
			"%{Library.opencv_video}",
			"%{Library.opencv_videoio}",
			"%{Library.opencv_highgui}",
			"%{Library.opencv_imgproc}",
			"%{Library.opencv_imgcodecs}"
			
		}