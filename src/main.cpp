#include "compat.h"
#include "loop.h"

#ifdef __NDS__
#include <filesystem.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

static void loop_cb(void *arg) {
	GameLoop* loop = (GameLoop*)arg;
	if (!loop->iterate())
		emscripten_cancel_main_loop();
}

#endif

int main(int argc, char* argv[])
{
	GameLoop* loop = NULL;
	int retval = EXIT_SUCCESS;

	(void)argc;
	(void)argv;

#ifdef __NDS__
	nitroFSInit(NULL);
#endif

	/* Initialize the game */
	loop = new GameLoop();
	if (loop->init()) {
#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop_arg(loop_cb, loop, 0, 1);
#else
		while (loop->iterate()) {
			SDL_Delay(10);
		}
#endif
	} else {
		retval = EXIT_FAILURE;
	}
	delete loop;

#ifdef __NDS__
	if (retval == EXIT_FAILURE)
		while (1);
#endif

	return retval;
}
