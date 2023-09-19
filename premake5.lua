workspace "TriangulateOBJ"
    configurations { "Debug", "Release" }
    platforms { "x86", "x64" }

    location "./build"

    filter { "system:windows", "configurations:Debug" }
        defines { "DEBUG" }
        symbols "On"

    filter { "system:windows", "configurations:Release" }
        defines { "NDEBUG" }
        optimize "On"

project "TriangulateOBJ"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir "%{wks.location}/bin/%{cfg.buildcfg}/%{cfg.platform}"
    objdir "%{wks.location}/obj/%{cfg.buildcfg}/%{cfg.platform}"

    files {"**.h", "**.cpp" }

	defines "_CRT_SECURE_NO_WARNINGS"

    filter { "configurations:Debug" }
        targetname "TriangulateOBJ"

    filter { "configurations:Release" }
        targetname "TriangulateOBJ"

    filter { "platforms:x86" }
        architecture "x86"

    filter { "platforms:x64" }
        architecture "x64"