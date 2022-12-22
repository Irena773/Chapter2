#pragma once
#include "SDL.h"
#include <string>

class Game {
public:
	
	
	bool Initialize();
	void RunLoop();
	void Shutdown();

	SDL_Texture* GetTexture(const std::string& fileName);

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	Uint32 mTicksCount;
	bool mIsRunning;
};
