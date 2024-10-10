project "ozz"
    kind "StaticLib"
    language "C"
    staticruntime "Off"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/".. outputdir .. "/%{prj.name}")

    files
    {
        "include/**.h",
        "src/**.h",
        "src/**.cc",
        "src/**.cpp"
    }
    includedirs
    {
        "include",
        "src"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        symbols "Off"

    filter "configurations:Dist"
        runtime "Release"
        optimize "On"
        symbols "Off"
