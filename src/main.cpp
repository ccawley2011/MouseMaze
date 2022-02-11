#ifdef __NDS__
#include <filesystem.h>
#endif

#include "compat.h"
#include "constants.h"
#include "render.h"
#include "loop.h"

int main(int argc, char* argv[])
{
	BaseRenderer* renderer = NULL;
	GameLoop* loop = NULL;
	int retval = EXIT_FAILURE;

	(void)argc;
	(void)argv;

#ifdef __NDS__
	nitroFSInit(NULL);
#endif

	/* Initialize SDL */
	renderer = new Renderer();
	if (renderer->init("Mouse Maze", SCREEN_X, SCREEN_Y)) {
		loop = new GameLoop(renderer);
		if (loop->init()) {
			loop->run();
		} else {
			renderer->msgBox("Couldn't initialize game: %s", SDL_GetError());
		}
	} else {
		renderer->msgBox("Couldn't initialize SDL: %s", SDL_GetError());
	}

	delete loop;
	delete renderer;

#ifdef __NDS__
	if (retval == EXIT_FAILURE)
		while (1);
#endif

	return retval;
}
