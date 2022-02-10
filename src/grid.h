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
