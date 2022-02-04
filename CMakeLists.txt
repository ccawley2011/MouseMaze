cmake_minimum_required(VERSION 3.0.2)
project(MouseMaze LANGUAGES CXX)

set(SDL_VERSION AUTO CACHE STRING "Specify which SDL version to use")
set_property(CACHE SDL_VERSION PROPERTY STRINGS AUTO SDL1 SDL2)

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
	src/constants.h
	src/font.cpp
	src/font.h
	src/grid.cpp
	src/grid.h
	src/palette.h
	src/render.cpp
	src/render.h
	src/loop.cpp
	src/loop.h
)

if(SDL2_FOUND)
	target_link_libraries(MouseMaze PRIVATE SDL2::SDL2main SDL2::SDL2)
endif(SDL2_FOUND)
if(SDL_FOUND)
	target_include_directories(MouseMaze PUBLIC ${SDL_INCLUDE_DIR})
	target_link_libraries(MouseMaze PRIVATE ${SDL_LIBRARY})
endif(SDL_FOUND)

if (MINGW)
	target_link_options(MouseMaze PRIVATE -static-libgcc -static-libstdc++)
endif()

if(NINTENDO_DS)
	target_link_libraries(MouseMaze PRIVATE filesystem fat mm9)
	nds_create_rom(MouseMaze
		NAME "Mouse Maze"
		ROMFS ${CMAKE_CURRENT_SOURCE_DIR}/res
	)
endif()
