#ifndef GRID_H
#define GRID_H

#include "compat.h"
#include "constants.h"

class BaseRenderer;
class BaseSprite;
class Mouse;

enum {
	TILE_LEFT_1      = 0,
	TILE_RIGHT_1     = 1,
	TILE_UP_1        = 2,
	TILE_DOWN_1      = 3,
	TILE_START       = 4,

	TILE_LEFT_2      = 6,
	TILE_RIGHT_2     = 7,
	TILE_UP_2        = 8,
	TILE_DOWN_2      = 9,
	TILE_END_1       = 10,

	TILE_LEFT_3      = 12,
	TILE_RIGHT_3     = 13,
	TILE_UP_3        = 14,
	TILE_DOWN_3      = 15,
	TILE_END_2       = 16,

	TILE_FIXED_LEFT  = 18,
	TILE_FIXED_RIGHT = 19,
	TILE_FIXED_UP    = 20,
	TILE_FIXED_DOWN  = 21,
	TILE_BLANK       = 22,

	TILE_PLUS_1      = 24,
	TILE_MINUS_1     = 25,
	TILE_PLUS_2      = 26,
	TILE_MINUS_2     = 27,
	TILE_SPIKES      = 28,
};

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
	void loadLevel(int level);
	void renderMouse(Mouse* mouse, int x, int y);
	void renderTile(int tile, int x, int y);
	void selectTile(int tile);
	void moveSelector(int x, int y);
};

#endif
