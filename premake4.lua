-- A solution contains projects, and defines the available configurations
solution "ULE"

    ------------------------------------------------------------------
	-- setup common settings
	------------------------------------------------------------------
	configurations { "Debug", "Release" }
	--flags { "ExtraWarnings", "FatalWarnings", "FloatFast" }
	--includedirs { "Source" }
	location "build"
	includedirs {"src"}

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

    -- it950x driver
    project "driver"
        kind "StaticLib"
        language "C"
        targetdir "build/lib"
        --includedirs {"driver", "src/driver"}
        files { "src/driver/*.h", "src/driver/*.c" }

    -- ule protocol lib
    project "ule"
        kind "StaticLib"
        language "C"
        targetdir "build/lib"
        files { "src/ule/*.h", "src/ule/*.c" }
        --includedirs {"ule", "src/ule", "src/driver", "src/util"}
        links {"driver", "util"}

    -- utility lib
    project "util"
        kind "StaticLib"
        language "C"
        targetdir "build/lib"
        --includedirs {"util", "src/util"}
        files {"src/util/*.h", "src/util/*.c"}

    -- testkit sample
    project "sample"
        kind "ConsoleApp"
        language "C"
        targetdir "build/bin"
        files {"src/sample/*.h", "src/sample/*.c"}
        --includedirs {"src/driver", "src/util", "src/ule"}
        links {"driver", "util", "ule"}

    -- testcase
    project "test"
        kind "ConsoleApp"
        language "C"
        targetdir "build/bin"
        files {"src/test/*.h", "src/test/*.c"}
        links {"ule", "util"}
