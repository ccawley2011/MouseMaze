#include "loop.h"
#include "constants.h"
#include "font.h"
#include "grid.h"
#include "render.h"

GameLoop::GameLoop(BaseRenderer* renderer) : _renderer(renderer), _shouldQuit(false), _title(NULL) {
	_font = new Font(0x100);
	_grid = new Grid(renderer, GRID_X, GRID_Y, 10, 6);
}

GameLoop::~GameLoop() {
	delete _grid;
	delete _font;
}

bool GameLoop::init() {
	if (!_grid->init()) {
		return false;
	}

	if (!_font->load(DATA_PATH "unifont-13.0.06.hex")) {
		return false;
	}

	_title = _font->createSprite(_renderer, "Mouse Maze", 0xFF, 0xFF, 0xFF);
	if (!_title) {
		return false;
	}

	return true;
}

void GameLoop::handleEvent(SDL_Event *event) {
	switch (event->type) {
	case SDL_QUIT:
		_shouldQuit = true;
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (_grid->handleClick(event->button.x, event->button.y))
			break;
		break;
	}
}

void GameLoop::render()
{
	/* Erase all the sprites if necessary */
	_renderer->clear();

	_grid->render();

	_title->render(TITLE_X, TITLE_Y);

	/* Update the screen! */
	_renderer->present();
}

void GameLoop::run() {
	SDL_Event event;
	while (!_shouldQuit) {
		while (SDL_PollEvent(&event)) {
			handleEvent(&event);
		}
		render();
		SDL_Delay(10);
	}
}
