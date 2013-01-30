-- A solution contains projects, and defines the available configurations
solution "ULE"

    ------------------------------------------------------------------
	-- setup common settings
	------------------------------------------------------------------
	configurations { "Debug", "Release" }
	--flags { "ExtraWarnings", "FatalWarnings", "FloatFast" }
	location "build"
	includedirs {"src"}
    targetdir "build/bin"

	------------------------------------------------------------------
	-- setup the build configs
	------------------------------------------------------------------
	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }
		targetsuffix "_d"
        buildoptions { "-Wall", "-std=c99" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }
        buildoptions { "-Wall", "-std=c99" }

	------------------------------------------------------------------
	-- project settings
	------------------------------------------------------------------
    -- it950x driver
    project "driver"
        kind "StaticLib"
        language "C"
        files { "src/driver/*.h", "src/driver/*.c" }

    -- ule protocol lib
    project "ule"
        kind "StaticLib"
        language "C"
        files { "src/ule/*.h", "src/ule/*.c" }
        --links {"driver", "util"}

    -- utility lib
    project "util"
        kind "StaticLib"
        language "C"
        files {"src/util/*.h", "src/util/*.c"}

    -- testkit sample
    project "sample"
        kind "ConsoleApp"
        language "C"
        files {"src/sample/*.h", "src/sample/*.c"}
        links {"driver", "util", "ule"}

    -- testcase
    project "test"
        kind "ConsoleApp"
        language "C"
        files {"src/test/*.h", "src/test/*.c"}
        links {"ule", "util"}
