#include "grid.h"
#include "constants.h"
#include "render.h"

Grid::Grid(BaseRenderer* renderer, int x, int y, int width, int height) : _renderer(renderer), _x(x), _y(y), _width(width), _height(height), _tileSprite(0), _tiles(0), _tileCount(0) {
	_layout = new Uint8[_width * _height];
	SDL_memset(_layout, 0, _width * _height);
}

Grid::~Grid() {
	delete _selector;
	delete _tileSprite;
	delete _tiles;
	delete _layout;
}

bool Grid::init() {
	_tileSprite = _renderer->loadSprite(DATA_PATH "tiles.bmp", 0);
	if (!_tileSprite) {
		return false;
	}

	_selector = _renderer->loadSprite(DATA_PATH "selector.bmp", 0);
	if (!_selector) {
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

	_selector->render(_x + ((_currentTile % _width) * TILE_W) - 1, _y + ((_currentTile / _width) * TILE_H) - 1);
}

void Grid::renderTile(int tile, int x, int y) {
	if (tile >= _tileCount)
		return;

	_tileSprite->render(&_tiles[tile], x, y);
}

void Grid::selectTile(int tile) {
	_layout[tile]++;
	if (_layout[tile] == _tileCount)
		_layout[tile] = 0;
}

void Grid::moveSelector(int x, int y) {
	int lastPosition = _currentTile;

	_currentTile += x;
	if (_currentTile < 0)
		_currentTile += _width;
	else if (_currentTile / _width < lastPosition / _width)
		_currentTile += _width;
	else if (_currentTile / _width > lastPosition / _width)
		_currentTile -= _width;

	_currentTile += (y * _width);
	if (_currentTile < 0)
		_currentTile += _width * _height;
	else if (_currentTile >= _width * _height)
		_currentTile -= _width * _height;
}

bool Grid::handleMouseMove(Sint32 x, Sint32 y) {
	if (x < _x || y < _y || x >= _x + (_width * TILE_W) || y >= _y + (_height * TILE_H))
		return false;

	int tileX = ((x - _x) / TILE_W);
	int tileY = ((y - _y) / TILE_H);
	_currentTile = (tileY * _width) + tileX;

	return true;
}

bool Grid::handleMouseClick(Sint32 x, Sint32 y) {
	if (x < _x || y < _y || x >= _x + (_width * TILE_W) || y >= _y + (_height * TILE_H))
		return false;

	int tileX = ((x - _x) / TILE_W);
	int tileY = ((y - _y) / TILE_H);
	selectTile((tileY * _width) + tileX);

	return true;
}

bool Grid::handleKeyPress(Uint8 state, const SDL_Keysym &keysym) {
	switch (keysym.sym) {
	case SDLK_KP_1:
	case SDLK_END:
		if (state == SDL_PRESSED)
			moveSelector(-1, 1);
		return true;
	case SDLK_KP_2:
	case SDLK_DOWN:
		if (state == SDL_PRESSED)
			moveSelector(0, 1);
		return true;
	case SDLK_KP_3:
	case SDLK_PAGEDOWN:
		if (state == SDL_PRESSED)
			moveSelector(1, 1);
		return true;
	case SDLK_KP_4:
	case SDLK_LEFT:
		if (state == SDL_PRESSED)
			moveSelector(-1, 0);
		return true;
	case SDLK_KP_6:
	case SDLK_RIGHT:
		if (state == SDL_PRESSED)
			moveSelector(1, 0);
		return true;
	case SDLK_KP_7:
	case SDLK_HOME:
		if (state == SDL_PRESSED)
			moveSelector(-1, -1);
		return true;
	case SDLK_KP_8:
	case SDLK_UP:
		if (state == SDL_PRESSED)
			moveSelector(0, -1);
		return true;
	case SDLK_KP_9:
	case SDLK_PAGEUP:
		if (state == SDL_PRESSED)
			moveSelector(1, -1);
		return true;
	case SDLK_SPACE:
		if (state == SDL_PRESSED)
			selectTile(_currentTile);
		return true;
	}
	return false;
}

bool Grid::handleJoystickButton(Uint8 state, Uint8 button) {
	switch (button) {
	case JOYSTICK_BUTTON_A:
		if (state == SDL_PRESSED)
			selectTile(_currentTile);
		return true;
	}
	return false;
}

bool Grid::handleJoystickHat(Uint8 hat, Uint8 value) {
	int x = 0, y = 0;

	if (value & SDL_HAT_UP)
		y = -1;
	if (value & SDL_HAT_DOWN)
		y = 1;
	if (value & SDL_HAT_LEFT)
		x = -1;
	if (value & SDL_HAT_RIGHT)
		x = 1;

	moveSelector(x, y);

	return true;
}

#if SDL_VERSION_ATLEAST(2, 0, 0)

bool Grid::handleControllerButton(Uint8 state, Uint8 button) {
	switch (button) {
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		if (state == SDL_PRESSED)
			moveSelector(0, 1);
		return true;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		if (state == SDL_PRESSED)
			moveSelector(-1, 0);
		return true;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		if (state == SDL_PRESSED)
			moveSelector(1, 0);
		return true;
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		if (state == SDL_PRESSED)
			moveSelector(0, -1);
		return true;
	case SDL_CONTROLLER_BUTTON_A:
		if (state == SDL_PRESSED)
			selectTile(_currentTile);
		return true;
	}
	return false;
}

#endif