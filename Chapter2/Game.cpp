#include "Game.h"
#include "SDL_image.h"
#include <algorithm>
#include "Actor.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "BGSpriteComponent.h"


Game::Game()
	:mWindow(nullptr)
	, mRenderer(nullptr)
	, mIsRunning(true)
	, mUpdatingActors(false)
{

}
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
	LoadData();

	mTicksCount = SDL_GetTicks();
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

	mShip->ProcessKeyboard(state);
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

	//すべてのアクターを更新
	mUpdatingActors = true;
	for (auto actor : mActors) {
		actor->Update(deltaTime);
	}
	mUpdatingActors = false;

	//待ちになっていたアクターをmActorsに移動
	for (auto pending : mPendingActors) {
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();
	//死んだアクターを一時配列に追加
	std::vector<Actor*> deadActors;
	for (auto actor : mActors) {
		if (actor->GetState() == Actor::EDead) {
			deadActors.emplace_back(actor);
		}
	}
	//死んだアクターを消す(mActorsから削除される)
	for (auto actor : deadActors) {
		delete actor;
	}

}

void Game::GenerateOutput() {
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);
	SDL_RenderClear(mRenderer);
	for (auto sprite : mSprites)
	{
		sprite->Draw(mRenderer);
	}
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

void Game::AddActor(Actor* actor) {
	//アクターの更新中なら待ちに追加
	if (mUpdatingActors) {
		mPendingActors.emplace_back(actor);
	}
	else {
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor) {

	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end()) {
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end()) {
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

void Game::AddSprite(SpriteComponent* sprite) {
	//ソート済みの配列で挿入点を見つける
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (; iter != mSprites.end(); ++iter) {
		if (myDrawOrder < (*iter)->GetDrawOrder()) {
			break;
		}
	}
	//イテレータの位置の前に要素を挿入する
	mSprites.insert(iter, sprite);
	
}
void Game::RemoveSprite(SpriteComponent* sprite)
{
	// (We can't swap because it ruins ordering)
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}

void Game::LoadData()
{
	// Create player's ship
	mShip = new Ship(this);
	mShip->SetPosition(Vector2(100.0f, 384.0f));
	mShip->SetScale(1.5f);

	// Create actor for the background (this doesn't need a subclass)
	Actor* temp = new Actor(this);
	temp->SetPosition(Vector2(512.0f, 384.0f));
	// Create the "far back" background
	BGSpriteComponent* bg = new BGSpriteComponent(temp);
	bg->SetScreenSize(Vector2(1024.0f, 768.0f));
	std::vector<SDL_Texture*> bgtexs = {
		GetTexture("Assets/Farback01.png"),
		GetTexture("Assets/Farback02.png")
	};
	bg->SetBGTextures(bgtexs);
	bg->SetScrollSpeed(-100.0f);
	// Create the closer background
	bg = new BGSpriteComponent(temp, 50);
	bg->SetScreenSize(Vector2(1024.0f, 768.0f));
	bgtexs = {
		GetTexture("Assets/Stars.png"),
		GetTexture("Assets/Stars.png")
	};
	bg->SetBGTextures(bgtexs);
	bg->SetScrollSpeed(-200.0f);
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// Destroy textures
	for (auto i : mTextures)
	{
		SDL_DestroyTexture(i.second);
	}
	mTextures.clear();
}