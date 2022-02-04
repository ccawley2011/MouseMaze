#ifndef GRID_H
#define GRID_H

#include <SDL.h>

class BaseRenderer;
class BaseSprite;

class Grid {
public:
	Grid(BaseRenderer* renderer, int x, int y, int width, int height);
	~Grid();

	bool init();
	void render();
	bool handleClick(int x, int y);

private:
	BaseRenderer* _renderer;
	BaseSprite* _tileSprite;
	SDL_Rect* _tiles;
	Uint8* _layout;
	int _tileCount;
	int _x;
	int _y;
	int _width;
	int _height;

	void renderTile(int tile, int x, int y);
};

#endif
