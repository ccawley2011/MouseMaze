#ifndef GRID_H
#define GRID_H

#include "compat.h"
#include "constants.h"

class BaseRenderer;
class BaseSprite;
class Mouse;

class Grid {
	friend class Mouse;
public:
	Grid(BaseRenderer* renderer, int x, int y, int width, int height);
	~Grid();

	bool init();
	void render();
	bool handleMouseMove(Sint32 x, Sint32 y);
	bool handleMouseClick(Sint32 x, Sint32 y);
	bool handleKeyPress(Uint8 state, const SDL_Keysym &keysym);
	bool handleJoystickButton(Uint8 state, Uint8 button);
	bool handleJoystickHat(Uint8 value);
#if SDL_VERSION_ATLEAST(2, 0, 0)
	bool handleControllerButton(Uint8 state, Uint8 button);
#endif

	int getXMin() const { return _x; }
	int getYMin() const { return _y; }
	int getXMax() const { return _x + ((_width - 1) * TILE_W); }
	int getYMax() const { return _y + ((_height - 1) * TILE_H); }

private:
	BaseRenderer* _renderer;
	BaseSprite* _tileSprite;
	BaseSprite* _mouseSprite;
	BaseSprite* _selector;
	Mouse* _mice[64];
	SDL_Rect* _tiles;
	Uint8* _layout;
	int _tileCount;
	int _currentTile;
	int _x;
	int _y;
	int _width;
	int _height;

protected:
	void renderMouse(Mouse* mouse, int x, int y);
	void renderTile(int tile, int x, int y);
	void selectTile(int tile);
	void moveSelector(int x, int y);
};

#endif
