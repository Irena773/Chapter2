#pragma once
#include "SDL.h"

class Game {
public:
	
	
	bool Initialize();
	void RunLoop();
	void Shutdown();

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	Uint32 mTicksCount;
	bool mIsRunning;
};
