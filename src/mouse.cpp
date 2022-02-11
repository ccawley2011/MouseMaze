#include "mouse.h"
#include "grid.h"

void Mouse::render() {
	Uint32 ticks = SDL_GetTicks();

	if (_lastAnimTicks + (1000 / 30) < ticks) {
		nextFrame();
		_lastAnimTicks = ticks;
	}

	if (_lastMovementTicks + (1000 / 60) < ticks) {
		move();
		_lastMovementTicks = ticks;
	}

	_grid->renderMouse(this, _x, _y);
}

void Mouse::nextFrame() {
	_frame++;
	if (_frame > 3)
		_frame = 0;
}

void Mouse::move() {
	if ((_x != _lastTileX && (_x - _grid->getXMin()) % TILE_W == 0) ||
            (_y != _lastTileY && (_y - _grid->getYMin()) % TILE_H == 0)) {
		int tileX = ((_x - _grid->getXMin()) / TILE_W);
		int tileY = ((_y - _grid->getYMin()) / TILE_H);

		switch (_grid->getTileAt(tileX, tileY)) {
		case TILE_LEFT_1:
		case TILE_LEFT_2:
		case TILE_LEFT_3:
		case TILE_FIXED_LEFT:
			_direction = MOUSE_LEFT;
			break;
		case TILE_RIGHT_1:
		case TILE_RIGHT_2:
		case TILE_RIGHT_3:
		case TILE_FIXED_RIGHT:
			_direction = MOUSE_RIGHT;
			break;
		case TILE_UP_1:
		case TILE_UP_2:
		case TILE_UP_3:
		case TILE_FIXED_UP:
			_direction = MOUSE_UP;
			break;
		case TILE_DOWN_1:
		case TILE_DOWN_2:
		case TILE_DOWN_3:
		case TILE_FIXED_DOWN:
			_direction = MOUSE_DOWN;
			break;
		}

		_lastTileX = _x;
		_lastTileY = _y;
	} else if (_direction == MOUSE_LEFT) {
		if (_x == _grid->getXMin()) {
			_direction = MOUSE_UP;
		}
	} else if (_direction == MOUSE_RIGHT) {
		if (_x == _grid->getXMax()) {
			_direction = MOUSE_DOWN;
		}
	} else if (_direction == MOUSE_UP) {
		if (_y == _grid->getYMin()) {
			_direction = MOUSE_RIGHT;
		}
	} else if (_direction == MOUSE_DOWN) {
		if (_y == _grid->getYMax()) {
			_direction = MOUSE_LEFT;
		}
	}

	if (_direction == MOUSE_LEFT) {
		_x = SDL_max(_x - 1, _grid->getXMin());
	} else if (_direction == MOUSE_RIGHT) {
		_x = SDL_min(_x + 1, _grid->getXMax());
	} else if (_direction == MOUSE_UP) {
		_y = SDL_max(_y - 1, _grid->getYMin());
	} else if (_direction == MOUSE_DOWN) {
		_y = SDL_min(_y + 1, _grid->getYMax());
	}
}
