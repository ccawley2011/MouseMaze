cmake_minimum_required(VERSION 3.0.2...3.10)
project(MouseMaze LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_STANDARD 98)

set(SDL_VERSION AUTO CACHE STRING "Specify which SDL version to use")
set_property(CACHE SDL_VERSION PROPERTY STRINGS AUTO SDL1 SDL2)

if (EMSCRIPTEN)
	# HACK: Emscripten requires extra parameters on first use that
	# aren't handled by the detection scripts
	include(CheckCXXCompilerFlag)
	check_cxx_compiler_flag("-s USE_SDL=2" HAS_SDL2_EMSCRIPTEN)
endif()

if(SDL_VERSION STREQUAL "AUTO")
	find_package(SDL2)
	if(NOT SDL2_FOUND)
		find_package(SDL REQUIRED)
	endif(NOT SDL2_FOUND)
elseif(SDL_VERSION STREQUAL "SDL2")
	find_package(SDL2 REQUIRED)
elseif(SDL_VERSION STREQUAL "SDL1")
	find_package(SDL REQUIRED)
else()
	message(FATAL_ERROR "Invalid value for SDL_VERSION: ${SDL_VERSION}")
endif()

add_executable(MouseMaze WIN32
	src/main.cpp
	src/compat.h
	src/constants.h
	src/font.cpp
	src/font.h
	src/grid.cpp
	src/grid.h
	src/mouse.cpp
	src/mouse.h
	src/palette.h
	src/render.cpp
	src/render.h
	src/loop.cpp
	src/loop.h
)

if(SDL2_FOUND)
	target_include_directories(MouseMaze PUBLIC ${SDL2_INCLUDE_DIR})
	target_link_libraries(MouseMaze PRIVATE ${SDL2_LIBRARIES})
endif(SDL2_FOUND)
if(SDL_FOUND)
	target_include_directories(MouseMaze PUBLIC ${SDL_INCLUDE_DIR})
	target_link_libraries(MouseMaze PRIVATE ${SDL_LIBRARY})
endif(SDL_FOUND)

if(NOT MSVC)
	target_compile_options(MouseMaze PRIVATE -Wall -Wextra -pedantic)
endif()

if(WIN32)
	if(MINGW)
		target_link_options(MouseMaze PRIVATE -static-libgcc -static-libstdc++)
	endif()

	install(TARGETS MouseMaze DESTINATION .)
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION .)

	cmake_policy(SET CMP0087 NEW)

	install(CODE [[
		if(CMAKE_OBJDUMP)
			string(REGEX REPLACE "objdump.exe" "" MINGW_PATH ${CMAKE_OBJDUMP})
		endif()

		file(GET_RUNTIME_DEPENDENCIES
			EXECUTABLES $<TARGET_FILE:MouseMaze>
			RESOLVED_DEPENDENCIES_VAR _r_deps
			PRE_EXCLUDE_REGEXES ".*"
			PRE_INCLUDE_REGEXES "^sdl*"
			DIRECTORIES ${MINGW_PATH}
		)
		file(INSTALL DESTINATION ${CMAKE_INSTALL_PREFIX} MESSAGE_LAZY FILES ${_r_deps})
	]])

	set(CPACK_GENERATOR "ZIP")
	if(CMAKE_CL_64)
		set(CPACK_PACKAGE_FILE_NAME "mousemaze-win64")
	else()
		set(CPACK_PACKAGE_FILE_NAME "mousemaze-win32")
	endif()
endif()

if(EMSCRIPTEN)
	target_link_options(MouseMaze PRIVATE
		--preload-file "${CMAKE_SOURCE_DIR}/res@res/"
	)
	file(CREATE_LINK ${CMAKE_SOURCE_DIR}/pkg/index.html ${CMAKE_BINARY_DIR}/index.html COPY_ON_ERROR SYMBOLIC)

	# TODO: Use CPack
endif()

if(RISCOS)
	target_link_options(MouseMaze PRIVATE -static)
	riscos_elf2aif(MouseMaze OUTPUT_NAME MouseMaze,ff8)

	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/pkg/!MouseMaze/!Boot,feb
	              ${CMAKE_CURRENT_SOURCE_DIR}/pkg/!MouseMaze/!Run,feb
	              ${CMAKE_CURRENT_BINARY_DIR}/MouseMaze,ff8
	        DESTINATION !MouseMaze
	)
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res
	        DESTINATION !MouseMaze
	)

	# TODO: Use .zip archives instead
	set(CPACK_GENERATOR "TGZ")
	set(CPACK_PACKAGE_FILE_NAME "mousemaze-riscos")
endif()

if(NINTENDO_DS)
	target_link_libraries(MouseMaze PRIVATE filesystem fat mm9)
	nds_create_rom(MouseMaze
		NAME "Mouse Maze"
		ROMFS ${CMAKE_CURRENT_SOURCE_DIR}/res
	)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MouseMaze.nds DESTINATION .)
	set(CPACK_GENERATOR "ZIP")
	set(CPACK_PACKAGE_FILE_NAME "mousemaze-nds")
endif()

include(CPack)
