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
	if (_direction == MOUSE_LEFT) {
		_x = SDL_max(_x - 1, _grid->getXMin());
		if (_x == _grid->getXMin()) {
			_direction = MOUSE_UP;
		}
	} else if (_direction == MOUSE_RIGHT) {
		_x = SDL_min(_x + 1, _grid->getXMax());
		if (_x == _grid->getXMax()) {
			_direction = MOUSE_DOWN;
		}
	} else if (_direction == MOUSE_UP) {
		_y = SDL_max(_y - 1, _grid->getYMin());
		if (_y == _grid->getYMin()) {
			_direction = MOUSE_RIGHT;
		}
	} else if (_direction == MOUSE_DOWN) {
		_y = SDL_min(_y + 1, _grid->getYMax());
		if (_y == _grid->getYMax()) {
			_direction = MOUSE_LEFT;
		}
	}

}