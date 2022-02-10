#ifndef GRID_H
#define GRID_H

#include "compat.h"

class BaseRenderer;
class BaseSprite;

class Grid {
public:
	Grid(BaseRenderer* renderer, int x, int y, int width, int height);
	~Grid();

	bool init();
	void render();
	bool handleMouseMove(Sint32 x, Sint32 y);
	bool handleMouseClick(Sint32 x, Sint32 y);
	bool handleKeyPress(Uint8 state, const SDL_Keysym &keysym);
	bool handleJoystickButton(Uint8 state, Uint8 button);
	bool handleJoystickHat(Uint8 hat, Uint8 value);
#if SDL_VERSION_ATLEAST(2, 0, 0)
	bool handleControllerButton(Uint8 state, Uint8 button);
#endif

private:
	BaseRenderer* _renderer;
	BaseSprite* _tileSprite;
	BaseSprite* _selector;
	SDL_Rect* _tiles;
	Uint8* _layout;
	int _tileCount;
	int _currentTile;
	int _x;
	int _y;
	int _width;
	int _height;

	void renderTile(int tile, int x, int y);
	void selectTile(int tile);
	void moveSelector(int x, int y);
};

#endif
