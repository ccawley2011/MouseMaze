#ifndef LOOP_H
#define LOOP_H

#include <SDL.h>

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

	BaseRenderer *_renderer;
	bool _shouldQuit;

	Grid* _grid;

	Font* _font;
	BaseSprite* _title;
};

#endif
