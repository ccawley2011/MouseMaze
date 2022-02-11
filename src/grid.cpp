#include "grid.h"
#include "constants.h"
#include "mouse.h"
#include "render.h"

struct TileInfo {
	int tile;
	int x;
	int y;
};

struct LevelInfo {
	const TileInfo *tiles;
	size_t tiles_count;
};

static const TileInfo tiles_level_1[] = {
	{ TILE_START,       1, 1 },
	{ TILE_UP_1,        4, 1 },
	{ TILE_FIXED_UP,    5, 1 },
	{ TILE_LEFT_1,      3, 2 },
	{ TILE_FIXED_LEFT,  3, 3 },
	{ TILE_FIXED_RIGHT, 6, 2 },
	{ TILE_RIGHT_1,     6, 3 },
	{ TILE_FIXED_DOWN,  4, 4 },
	{ TILE_DOWN_1,      5, 4 },
	{ TILE_RIGHT_1,     4, 0 },
	{ TILE_UP_1,        0, 2 },
	{ TILE_DOWN_1,      9, 3 },
	{ TILE_LEFT_1,      5, 5 },
	{ TILE_END_1,       8, 3 },
	{ TILE_SPIKES,      2, 2 }
};

static const TileInfo tiles_level_2[] = {
	{ TILE_START,       1, 1 },
	{ TILE_DOWN_1,      4, 1 },
	{ TILE_FIXED_DOWN,  5, 1 },
	{ TILE_RIGHT_1,     3, 2 },
	{ TILE_FIXED_RIGHT, 3, 3 },
	{ TILE_FIXED_LEFT,  6, 2 },
	{ TILE_LEFT_1,      6, 3 },
	{ TILE_FIXED_UP,    4, 4 },
	{ TILE_UP_1,        5, 4 }
};

static const LevelInfo levels[] = {
	{ tiles_level_1, SDL_arraysize(tiles_level_1) },
	{ tiles_level_2, SDL_arraysize(tiles_level_2) }
};

Grid::Grid(BaseRenderer* renderer, int x, int y, int width, int height) : _renderer(renderer), _tileSprite(NULL), _mouseSprite(NULL), _selector(NULL), _tiles(NULL), _tileCount(0), _currentTile(0), _x(x), _y(y), _width(width), _height(height), _miceReleased(0), _miceReturned(0), _score(0), _lastSpawnedTicks(0) {
	_layout = new Uint8[_width * _height];
	SDL_memset(_layout, TILE_BLANK, _width * _height);
	SDL_memset(_mice, 0, sizeof(_mice));
}

Grid::~Grid() {
	for (size_t i = 0; i < SDL_arraysize(_mice); i++) {
		delete _mice[i];
	}

	delete _selector;
	delete _mouseSprite;
	delete _tileSprite;
	delete _tiles;
	delete _layout;
}

bool Grid::init() {
	_tileSprite = _renderer->loadSprite(DATA_PATH "tiles.bmp", 0);
	if (!_tileSprite) {
		return false;
	}

	_mouseSprite = _renderer->loadSprite(DATA_PATH "mouse.bmp", 0);
	if (!_mouseSprite) {
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

	loadLevel(0);

	return true;
}

void Grid::loadLevel(int level) {
	const TileInfo *tiles = levels[level].tiles;
	size_t tiles_count = levels[level].tiles_count;

	SDL_memset(_layout, TILE_BLANK, _width * _height);

	for (size_t i = 0; i < tiles_count; i++) {
		_layout[(_width * tiles[i].y) + tiles[i].x] = tiles[i].tile;
	}
}

void Grid::render() {
	dispatchMice();

	for (int i = 0; i < _height; i++) {
		for (int j = 0; j < _width; j++) {
			int tile = (i * _width) + j;
			renderTile(_layout[tile], _x + (j * TILE_W), _y + (i * TILE_H));
		}
	}

	for (size_t i = 0; i < SDL_arraysize(_mice); i++) {
		if (!_mice[i])
			continue;
		_mice[i]->render();

		if (_mice[i]->isDead()) {
			_miceReturned++;
			delete _mice[i];
			_mice[i] = NULL;
		} else if (_mice[i]->reachedGoal()) {
			_miceReturned++;
			_score += _mice[i]->getPoints();
			delete _mice[i];
			_mice[i] = NULL;
		}
	}

	_selector->render(_x + ((_currentTile % _width) * TILE_W) - 1, _y + ((_currentTile / _width) * TILE_H) - 1);
}

void Grid::dispatchMice() {
	Uint32 ticks = SDL_GetTicks();

	if (_lastSpawnedTicks + 500 > ticks) {
		return;
	}

	_lastSpawnedTicks = ticks;

	for (int i = 0; i < _height; i++) {
		for (int j = 0; j < _width; j++) {
			int tile = (i * _width) + j;
			if (_layout[tile] != TILE_START)
				continue;
			if (_miceReleased == SDL_arraysize(_mice))
				break;
			_mice[_miceReleased++] = new Mouse(this, MOUSE_DOWN, _x + (j * TILE_W), _y + (i * TILE_H));
		}
	}
}

void Grid::renderMouse(Mouse* mouse, int x, int y) {
	SDL_Rect srcRect;
	srcRect.x = mouse->getFrame() * MOUSE_W;
	srcRect.y = mouse->getDirection() * MOUSE_H;
	srcRect.w = MOUSE_W;
	srcRect.h = MOUSE_H;
	_mouseSprite->render(&srcRect, x, y);
}

void Grid::renderTile(int tile, int x, int y) {
	if (tile >= _tileCount)
		return;

	_tileSprite->render(&_tiles[tile], x, y);
}

void Grid::selectTile(int tile) {
	switch (_layout[tile]) {
	case TILE_LEFT_1:  _layout[tile] = TILE_UP_1;    break;
	case TILE_RIGHT_1: _layout[tile] = TILE_DOWN_1;  break;
	case TILE_UP_1:    _layout[tile] = TILE_RIGHT_1; break;
	case TILE_DOWN_1:  _layout[tile] = TILE_LEFT_1;  break;

	case TILE_LEFT_2:  _layout[tile] = TILE_UP_2;    break;
	case TILE_RIGHT_2: _layout[tile] = TILE_DOWN_2;  break;
	case TILE_UP_2:    _layout[tile] = TILE_RIGHT_2; break;
	case TILE_DOWN_2:  _layout[tile] = TILE_LEFT_2;  break;

	case TILE_LEFT_3:  _layout[tile] = TILE_UP_3;    break;
	case TILE_RIGHT_3: _layout[tile] = TILE_DOWN_3;  break;
	case TILE_UP_3:    _layout[tile] = TILE_RIGHT_3; break;
	case TILE_DOWN_3:  _layout[tile] = TILE_LEFT_3;  break;
	}
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
	default:
		break;
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

bool Grid::handleJoystickHat(Uint8 value) {
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
