#ifndef _MOUSE_H
#define _MOUSE_H

#include "compat.h"

class Grid;

enum {
	MOUSE_LEFT = 0,
	MOUSE_DOWN = 1,
	MOUSE_RIGHT = 2,
	MOUSE_UP = 3
};

class Mouse {
public:
	Mouse(Grid* grid, int direction = MOUSE_DOWN, int x = 0, int y = 0) : _grid(grid), _lastAnimTicks(0), _lastMovementTicks(0), _direction(direction), _frame(0), _x(x), _y(y), _lastTileX(x), _lastTileY(y), _goal(false), _dead(false) {}
	~Mouse() {}

	void render();

	int getDirection() const { return _direction; }
	int getFrame() const { return _frame; }
	bool reachedGoal() const { return _goal; }
	bool isDead() const { return _dead; }
	int getPoints() const { return 1; }

private:
	Grid* _grid;
	Uint32 _lastAnimTicks;
	Uint32 _lastMovementTicks;
	int _direction;
	int _frame;
	int _x;
	int _y;
	int _lastTileX;
	int _lastTileY;
	bool _goal;
	bool _dead;

	void nextFrame();
	void move();
};

#endif
