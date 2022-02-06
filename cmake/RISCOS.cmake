if(DEFINED ENV{GCCSDK_INSTALL_CROSSBIN})
	set(GCCSDK_INSTALL_CROSSBIN $ENV{GCCSDK_INSTALL_CROSSBIN})
endif()
if(DEFINED ENV{GCCSDK_INSTALL_ENV})
	set(GCCSDK_INSTALL_ENV $ENV{GCCSDK_INSTALL_ENV})
endif()

# this one is important
SET(CMAKE_SYSTEM_NAME GNU)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER   ${GCCSDK_INSTALL_CROSSBIN}/arm-unknown-riscos-gcc)
SET(CMAKE_CXX_COMPILER ${GCCSDK_INSTALL_CROSSBIN}/arm-unknown-riscos-g++)

# where is the target environment
SET(CMAKE_FIND_ROOT_PATH  ${GCCSDK_INSTALL_ENV})

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(ENV{PKG_CONFIG_LIBDIR} ${GCCSDK_INSTALL_ENV}/lib/pkgconfig:${GCCSDK_INSTALL_ENV}/share/pkgconfig)

find_program(ELF2AIF NAMES elf2aif HINTS "${GCCSDK_INSTALL_CROSSBIN}")
if (NOT ELF2AIF)
	message(WARNING "Could not find elf2aif")
endif()

function(riscos_elf2aif target)
	cmake_parse_arguments(ELF2AIF "" "OUTPUT_NAME" "" ${ARGN})
	get_target_property(TARGET_OUTPUT_NAME ${target} OUTPUT_NAME)
	if(NOT TARGET_OUTPUT_NAME)
		set(TARGET_OUTPUT_NAME "${target}")
	endif()
	if(NOT ELF2AIF_OUTPUT_NAME)
		set(ELF2AIF_OUTPUT_NAME "${TARGET_OUTPUT_NAME},ff8")
	endif()

	add_custom_command(
		OUTPUT "${ELF2AIF_OUTPUT_NAME}"
		COMMAND "${ELF2AIF}" "$<TARGET_FILE:${target}>" "${ELF2AIF_OUTPUT_NAME}"
		DEPENDS ${target}
		VERBATIM
	)

	add_custom_target(
		"${target}_aif" ALL
		DEPENDS "${ELF2AIF_OUTPUT_NAME}"
	)
endfunction()

set(RISCOS TRUE)
