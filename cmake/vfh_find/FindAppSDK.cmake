#
# Copyright (c) 2015-2016, Chaos Software Ltd
#
# V-Ray For Houdini
#
# ACCESSIBLE SOURCE CODE WITHOUT DISTRIBUTION OF MODIFICATION LICENSE
#
# Full license text: https://github.com/ChaosGroup/vray-for-houdini/blob/master/LICENSE
#

string(TOLOWER "${CMAKE_HOST_SYSTEM_NAME}" _HOST_SYSTEM_NAME)

set(APPSDK_PATH "" CACHE PATH "V-Ray AppSDK root location")
set(APPSDK_VERSION "" CACHE STRING "V-Ray AppSDK version")

if(APPSDK_PATH)
	# if APPSDK_PATH is specified then just take it as root location for AppSDK
	set(_appsdk_root ${APPSDK_PATH})
else()
	if(NOT APPSDK_VERSION)
		message(STATUS "V-Ray AppSDK version NOT specified")
		set(_appsdk_root "")
	else()
		# no V-Ray AppSDK root path is passed to cmake
		if(SDK_PATH)
			# if vfh sdk path is given use it to deduce AppSDK root path based on version
			set(_appsdk_root "${SDK_PATH}/${_HOST_SYSTEM_NAME}/appsdk/appsdk${APPSDK_VERSION}")
		else()
			# otherwise search in default location for AppSDK
			set(_appsdk_root "$ENV{HOME}/src/appsdk_releases/${APPSDK_VERSION}/${_HOST_SYSTEM_NAME}")
		endif()
	endif()
endif()

message(STATUS "Searching V-Ray AppSDK path = ${_appsdk_root}")
message(STATUS "Searching V-Ray AppSDK version = ${APPSDK_VERSION}")

# check if path exists
if((NOT _appsdk_root) OR (NOT EXISTS ${_appsdk_root}))
	set(AppSDK_FOUND FALSE)
else()

	set(AppSDK_FOUND TRUE)
	set(AppSDK_INCLUDES "${_appsdk_root}/cpp/include")
	set(AppSDK_LIBRARIES "${_appsdk_root}/bin")

	if(WIN32)
		list(APPEND AppSDK_LIBRARIES "${_appsdk_root}/lib")
	endif()

endif()


# check if all paths exist
if(AppSDK_FOUND)
	foreach(loop_var IN ITEMS ${AppSDK_INCLUDES})
		if(NOT EXISTS ${loop_var})
			set(AppSDK_FOUND FALSE)
			break()
		endif()
	endforeach(loop_var)
	foreach(loop_var IN ITEMS ${AppSDK_LIBRARIES})
		if(NOT EXISTS ${loop_var})
			set(AppSDK_FOUND FALSE)
			break()
		endif()
	endforeach(loop_var)

	if (NOT EXISTS "${_appsdk_root}/bin/plugins")
		set(AppSDK_FOUND FALSE)
	endif()
	if (NOT EXISTS "${_appsdk_root}/bin/vrlservice")
		set(AppSDK_FOUND FALSE)
	endif()
endif()


if(NOT AppSDK_FOUND)
	message(STATUS "V-Ray AppSDK NOT found!")

	set(AppSDK_INCLUDES AppSDK_INCLUDES-NOTFOUND)
	set(AppSDK_LIBRARIES AppSDK_LIBRARIES-NOTFOUND)
endif()
