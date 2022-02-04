#include "grid.h"
#include "constants.h"
#include "render.h"

Grid::Grid(BaseRenderer* renderer, int x, int y, int width, int height) : _renderer(renderer), _x(x), _y(y), _width(width), _height(height), _tileSprite(0), _tiles(0), _tileCount(0) {
	_layout = new Uint8[_width * _height];
	SDL_memset(_layout, 0, _width * _height);
}

Grid::~Grid() {
	delete _tileSprite;
	delete _tiles;
	delete _layout;
}

bool Grid::init() {
	_tileSprite = _renderer->loadSprite(DATA_PATH "tiles.bmp", 0);
	if (!_tileSprite) {
		return false;
	}

	int w = _tileSprite->getWidth() / TILE_W;
	int h = _tileSprite->getHeight() / TILE_H;

	_tileCount = w * h;
	_tiles = new SDL_Rect[_tileCount];

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			int tile = (i * w) + j;
			if (tile >= _tileCount)
				continue;

			_tiles[tile].x = (j * TILE_W);
			_tiles[tile].y = (i * TILE_H);
			_tiles[tile].w = TILE_W;
			_tiles[tile].h = TILE_H;
		}
	}

	return true;
}

void Grid::render() {
	for (int i = 0; i < _height; i++) {
		for (int j = 0; j < _width; j++) {
			int tile = (i * _width) + j;
			renderTile(_layout[tile], _x + (j * TILE_W), _y + (i * TILE_H));
		}
	}
}

void Grid::renderTile(int tile, int x, int y) {
	if (tile >= _tileCount)
		return;

	_tileSprite->render(&_tiles[tile], x, y);
}

bool Grid::handleClick(int x, int y) {
	if (x < _x || y < _y || x > _x + (_width * TILE_W) || y > _y + (_height * TILE_H))
		return false;

	int tileX = ((x - _x) / TILE_W);
	int tileY = ((y - _y) / TILE_H);
	int tile = (tileY * _width) + tileX;

	_layout[tile]++;
	if (_layout[tile] == _tileCount)
		_layout[tile] = 0;

	return true;
}
