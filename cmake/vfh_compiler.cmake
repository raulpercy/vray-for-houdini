#
# Copyright (c) 2015-2016, Chaos Software Ltd
#
# V-Ray For Houdini
#
# ACCESSIBLE SOURCE CODE WITHOUT DISTRIBUTION OF MODIFICATION LICENSE
#
# Full license text: https://github.com/ChaosGroup/vray-for-houdini/blob/master/LICENSE
#

if(APPLE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9)
endif()


function(vfh_osx_flags _project_name)
	if(APPLE)
		# This sets search paths for modules like libvray.dylib
		# (no need for install_name_tool tweaks)
		set_target_properties(${_project_name}
			PROPERTIES
				INSTALL_RPATH "@loader_path;@executable_path")
	endif()
endfunction()


if(WIN32)
	# Houdini specific
	set(CMAKE_CXX_FLAGS "/wd4355 /w14996 /wd4800 /wd4244 /wd4305 /wd4251 /wd4275 /wd4396 /wd4018 /wd4267 /wd4146 /EHsc /GT /bigobj")
	# enable multi core compilation
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	set(CMAKE_CXX_FLAGS_DEBUG "/Od /MD /Zi /DNDEBUG")

else()
	set(CMAKE_CXX_FLAGS       "-std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG "-g -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

	if (APPLE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
		add_definitions(-DBOOST_NO_CXX11_RVALUE_REFERENCES)
	endif()

	add_definitions(-D__OPTIMIZE__)

	# Force color compiler output
	if(CMAKE_GENERATOR STREQUAL "Ninja")
		if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
			CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
		elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.0.0)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
		endif()
	endif()

	# disable App SDK warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof")

	# Houdini specific
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-attributes")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses -Wno-sign-compare -Wno-reorder -Wno-uninitialized -Wno-unused-parameter -Wno-deprecated -fno-strict-aliasing")

	# Houdini SDK / V-Ray SDK
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch -Wno-narrowing -Wno-int-to-pointer-cast")

	if (NOT APPLE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-literal-suffix -Wno-unused-local-typedefs")

		if(WITH_STATIC_LIBC)
			set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++")
		endif()

		set(CMAKE_CXX_CREATE_SHARED_LIBRARY "<CMAKE_CXX_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <SONAME_FLAG><TARGET_SONAME> -o <TARGET> -Wl,--start-group <OBJECTS> <LINK_LIBRARIES> -Wl,--end-group")
	endif()
endif()

message(STATUS "Using flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "  Release: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "  Debug:   ${CMAKE_CXX_FLAGS_DEBUG}")
