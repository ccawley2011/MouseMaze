#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef __NDS__
#define DATA_PATH "nitro:/"
#elif defined(__riscos__)
#define DATA_PATH "/<MouseMaze$Dir>/res/"
#else
#define DATA_PATH "res/"
#endif

#ifdef __NDS__
#define JOYSTICK_BUTTON_A 1
#define JOYSTICK_BUTTON_B 2
#else
#define JOYSTICK_BUTTON_A 0
#define JOYSTICK_BUTTON_B 1
#endif

#ifdef __NDS__
#define SCREEN_X 256
#define SCREEN_Y 192
#define GRID_X 8
#define GRID_Y 24
#define TITLE_X 88
#define TITLE_Y 4
#else
#define SCREEN_X 320
#define SCREEN_Y 240
#define GRID_X 40
#define GRID_Y 48
#define TITLE_X 120
#define TITLE_Y 24
#endif

#define TILE_W 24
#define TILE_H 24

#define MOUSE_W 24
#define MOUSE_H 24

#endif
