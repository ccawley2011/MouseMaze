#ifndef LOOP_H
#define LOOP_H

#include "compat.h"

class BaseRenderer;
class BaseSprite;
class Font;
class Grid;

class GameLoop {
public:
	GameLoop(BaseRenderer* renderer);
	~GameLoop();

	bool init();
	void run();

private:
	void handleEvent(SDL_Event* event);
	void render();
	void openJoystick(int index);
	void closeJoystick();

	SDL_Joystick* _joystick;
#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_GameController* _controller;
#endif

	BaseRenderer *_renderer;
	bool _shouldQuit;

	Grid* _grid;

	Font* _font;
	BaseSprite* _title;
};

#endif
