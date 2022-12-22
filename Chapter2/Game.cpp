#include "Game.h"
#include "SDL_image.h"
#include <string>

bool Game::Initialize() {

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		SDL_Log("SDLを初期化できません:%s", SDL_GetError());
		return false;
	}
	mWindow = SDL_CreateWindow("Game Programming in C++(Chapter2)", 100, 100, 1024, 768, 0);

	if (!mWindow) {
		SDL_Log("ウィンドウの作成に失敗しました:%s", SDL_GetError());
		return false;
	}
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer)
	{
		SDL_Log("レンダラーの作成に失敗しました: %s", SDL_GetError());
		return false;
	}
	if (IMG_Init(IMG_INIT_PNG) == 0) {
		SDL_Log("SDL_imageの初期化に失敗しました:%s", SDL_GetError());
		return false;
	}

	return true;
}

void Game::RunLoop() {
	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
}

void Game::UpdateGame() {
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();
}

void Game::GenerateOutput() {
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);
	SDL_RenderClear(mRenderer);

	SDL_RenderPresent(mRenderer);
}


//画像の読み込み処理
SDL_Texture* Game::GetTexture(const std::string& fileName) {
	//ファイルからロード
	SDL_Surface* surf = IMG_Load(fileName.c_str());
	if (!surf) {
		SDL_Log("テクスチャファイルのロードに失敗しました:%s", fileName.c_str());
		return nullptr;
	}

	//サーフェスからテクスチャを作成
	SDL_Texture* tex = SDL_CreateTextureFromSurface(mRenderer, surf);
	SDL_FreeSurface(surf);
	if (!tex) {
		SDL_Log("テクスチャへの変換に失敗しました:%s", fileName.c_str());
		return nullptr;
	}
	return tex;
}
void Game::Shutdown() {
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}