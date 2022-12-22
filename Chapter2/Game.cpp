#include "SDL.h"

int main(int argc, char** argv) {
	// ‰Šú‰»
	if (SDL_Init(SDL_INIT_VIDEO) != NULL) {
		// error
		SDL_Quit();
		return 1;
	}

	// Window‚ğ¶¬‚·‚é
	SDL_Window* window = SDL_CreateWindow(
		"Hello world",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		640, 480, 0
	);

	if (window == NULL) {
		// error
		SDL_Quit();
		return 1;
	}

	// 3•b‘Ò‚Â
	SDL_Delay(3000);

	// Œãˆ—
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}