workspace "Compiler-Tools"
	configurations {"Debug", "Release"}
	location "build"
	startproject "Test"

project "CTools"
	kind "SharedLib"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	
	files {"inc/**.hpp", "src/**.cpp"}
	
	includedirs{"inc/"}
	
	buildoptions{"-std=c++11", "-pthread"}
		
	filter "configurations:Debug"
		defines {"DEBUG", "COMPILE_DLL"}
		flags {"Symbols"}
	filter "configurations:Release"
		defines {"NDEBUG", "COMPILE_DLL"}
		optimize "On"
		
project "Test"
	kind "ConsoleApp"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	
	files{"test/**.hpp", "test/**.cpp"}
	
	links {"CTools"}
	
	includedirs{"inc/"}
	
	buildoptions{"-std=c++11", "-pthread"}
	
	filter "configurations:Debug"
		defines{"DEBUG"}
		flags {"Symbols"}
	filter "configurations:Release"
		defines{"NDEBUG"}
		optimize "On"