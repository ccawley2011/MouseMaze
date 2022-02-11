#include "loop.h"
#include "constants.h"
#include "font.h"
#include "grid.h"
#include "render.h"

GameLoop::GameLoop(BaseRenderer* renderer) : _joystick(NULL),
#if SDL_VERSION_ATLEAST(2, 0, 0)
		_controller(NULL),
#endif
		_renderer(renderer), _shouldQuit(false), _title(NULL) {
	_font = new Font(0x100);
	_grid = new Grid(renderer, GRID_X, GRID_Y, 10, 6);
}

GameLoop::~GameLoop() {
	delete _grid;
	delete _font;

	closeJoystick();
#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
#endif
	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

bool GameLoop::init() {
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
#else
	openJoystick(0);
#endif

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
	case SDL_MOUSEMOTION:
		if (_grid->handleMouseMove(event->button.x, event->button.y))
			break;
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (_grid->handleMouseClick(event->button.x, event->button.y))
			break;
		break;
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		if (_grid->handleKeyPress(event->key.state, event->key.keysym))
			break;
		break;
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		if (!_joystick)
			break;
		if (_grid->handleJoystickButton(event->jbutton.state, event->jbutton.button))
			break;
		break;
	case SDL_JOYHATMOTION:
		if (!_joystick)
			break;
		if (_grid->handleJoystickHat(event->jhat.value))
			break;
		break;
#if SDL_VERSION_ATLEAST(2, 0, 0)
	case SDL_CONTROLLERBUTTONDOWN:
	case SDL_CONTROLLERBUTTONUP:
		if (!_controller)
			break;
		if (_grid->handleControllerButton(event->cbutton.state, event->cbutton.button))
			break;
		break;
	case SDL_CONTROLLERDEVICEADDED:
		if (!_controller) {
			_controller = SDL_GameControllerOpen(event->cdevice.which);
		}
		break;
	case SDL_CONTROLLERDEVICEREMOVED:
		SDL_JoystickID id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(_controller));
		if (event->jdevice.which == id) {
			SDL_GameControllerClose(_controller);
			_controller = SDL_GameControllerOpen(0);
		}
		break;
#endif

	}
}

void GameLoop::openJoystick(int joystickIndex) {
#if SDL_VERSION_ATLEAST(2, 0, 0)
	if (SDL_IsGameController(joystickIndex)) {
		_controller = SDL_GameControllerOpen(joystickIndex);
	} else
#endif
	{
		_joystick = SDL_JoystickOpen(joystickIndex);
	}
}

void GameLoop::closeJoystick() {
#if SDL_VERSION_ATLEAST(2, 0, 0)
	if (_controller) {
		SDL_GameControllerClose(_controller);
		_controller = nullptr;
	}
#endif
	if (_joystick) {
		SDL_JoystickClose(_joystick);
		_joystick = nullptr;
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
