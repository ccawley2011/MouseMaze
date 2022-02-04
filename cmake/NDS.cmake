cmake_minimum_required(VERSION 3.7)
#include(${CMAKE_CURRENT_LIST_DIR}/devkitARM.cmake)
include($ENV{DEVKITPRO}/cmake/devkitARM.cmake)
include(dkp-custom-target)
include(dkp-embedded-binary)
include(dkp-asset-folder)

set(CMAKE_SYSTEM_PROCESSOR "armv5te")

set(NDS_ARCH_SETTINGS "-march=armv5te -mtune=arm946e-s")
set(NDS_COMMON_FLAGS  "${NDS_ARCH_SETTINGS} -ffunction-sections -fdata-sections -D__NDS__ -DARM9")
set(NDS_LIB_DIRS      "-L${DEVKITPRO}/libnds/lib -L${DEVKITPRO}/portlibs/nds/lib")

set(CMAKE_C_FLAGS_INIT   "${NDS_COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${NDS_COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${NDS_COMMON_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS_INIT "${NDS_ARCH_SETTINGS} ${NDS_LIB_DIRS} -specs=ds_arm9.specs")

set(CMAKE_FIND_ROOT_PATH
	${CMAKE_FIND_ROOT_PATH}
	${DEVKITPRO}/portlibs/nds
	${DEVKITPRO}/libnds
)

# Set pkg-config for the same
find_program(PKG_CONFIG_EXECUTABLE NAMES arm-none-eabi-pkg-config HINTS "${DEVKITPRO}/portlibs/nds/bin")
if (NOT PKG_CONFIG_EXECUTABLE)
	message(WARNING "Could not find arm-none-eabi-pkg-config: try installing nds-pkg-config")
endif()

find_program(NDS_NDSTOOL_EXE NAMES ndstool HINTS "${DEVKITPRO}/tools/bin")
if (NOT NDS_NDSTOOL_EXE)
	message(WARNING "Could not find ndstool: try installing ndstool")
endif()

find_file(NDS_DEFAULT_ICON NAMES icon.bmp HINTS "${DEVKITPRO}/libnds" NO_CMAKE_FIND_ROOT_PATH)
if (NOT NDS_DEFAULT_ICON)
	message(WARNING "Could not find default icon: try installing libnds")
endif()

function(nds_create_rom target)
	cmake_parse_arguments(NDSTOOL "" "NAME;SUBTITLE1;SUBTITLE2;ICON;ROMFS" "" ${ARGN})
	get_target_property(TARGET_OUTPUT_NAME ${target} OUTPUT_NAME)
	if(NOT TARGET_OUTPUT_NAME)
		set(TARGET_OUTPUT_NAME "${target}")
	endif()

	set(NDSTOOL_ARGS -c "${TARGET_OUTPUT_NAME}.nds" -9 "$<TARGET_FILE:${target}>")
	set(NDSTOOL_DEPS ${target})

	if (NOT DEFINED NDSTOOL_NAME)
		set(NDSTOOL_NAME "${CMAKE_PROJECT_NAME}")
	endif()
	if (NOT DEFINED NDSTOOL_SUBTITLE1)
		set(NDSTOOL_SUBTITLE1 "Built with devkitARM")
	endif()
	if (NOT DEFINED NDSTOOL_SUBTITLE2)
		set(NDSTOOL_SUBTITLE2 "http://devkitpro.org")
	endif()
	if (NOT DEFINED NDSTOOL_ICON)
		set(NDSTOOL_ICON "${NDS_DEFAULT_ICON}")
	endif()
	list(APPEND NDSTOOL_ARGS -b "${NDSTOOL_ICON}" "${NDSTOOL_NAME}\;${NDSTOOL_SUBTITLE1}\;${NDSTOOL_SUBTITLE2}")
	list(APPEND NDSTOOL_DEPS "${NDSTOOL_ICON}")

	if (DEFINED NDSTOOL_ROMFS)
		if (TARGET "${NDSTOOL_ROMFS}")
			get_target_property(_folder "${NDSTOOL_ROMFS}" DKP_ASSET_FOLDER)
			if (NOT _folder)
				message(FATAL_ERROR "nds_create_rom: not a valid asset target")
			endif()
			list(APPEND NDSTOOL_ARGS -d "${_folder}")
			list(APPEND NDSTOOL_DEPS ${NDSTOOL_ROMFS} $<TARGET_PROPERTY:${NDSTOOL_ROMFS},DKP_ASSET_FILES>)
		else()
			if (NOT IS_ABSOLUTE "${NDSTOOL_ROMFS}")
				set(NDSTOOL_ROMFS "${CMAKE_CURRENT_LIST_DIR}/${NDSTOOL_ROMFS}")
			endif()
			if (NOT IS_DIRECTORY "${NDSTOOL_ROMFS}")
				message(FATAL_ERROR "nds_create_rom: cannot find romfs dir: ${NDSTOOL_ROMFS}")
			endif()
			list(APPEND NDSTOOL_ARGS -d "${NDSTOOL_ROMFS}")
		endif()
	endif()

	add_custom_command(
		OUTPUT "${TARGET_OUTPUT_NAME}.nds"
		COMMAND "${NDS_NDSTOOL_EXE}" ${NDSTOOL_ARGS}
		DEPENDS ${NDSTOOL_DEPS}
		VERBATIM
	)

	add_custom_target(
		"${TARGET_OUTPUT_NAME}_nds" ALL
		DEPENDS "${target}.nds"
	)
endfunction()

set(NINTENDO_DS TRUE)

set(NDS_ROOT ${DEVKITPRO}/libnds)

set(NDS_STANDARD_LIBRARIES "-lnds9")
set(CMAKE_C_STANDARD_LIBRARIES "${NDS_STANDARD_LIBRARIES}" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD_LIBRARIES "${NDS_STANDARD_LIBRARIES}" CACHE STRING "" FORCE)
set(CMAKE_ASM_STANDARD_LIBRARIES "${NDS_STANDARD_LIBRARIES}" CACHE STRING "" FORCE)

#for some reason cmake (3.14.3) doesn't appreciate having \" here
set(NDS_STANDARD_INCLUDE_DIRECTORIES "${NDS_ROOT}/include")
set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES "${NDS_STANDARD_INCLUDE_DIRECTORIES}" CACHE STRING "")
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES "${NDS_STANDARD_INCLUDE_DIRECTORIES}" CACHE STRING "")
set(CMAKE_ASM_STANDARD_INCLUDE_DIRECTORIES "${NDS_STANDARD_INCLUDE_DIRECTORIES}" CACHE STRING "")
