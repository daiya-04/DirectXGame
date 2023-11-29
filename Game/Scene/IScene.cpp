#include "IScene.h"

// タイトルシーンで初期化
int IScene::sSceneNo_ = kTITLE;
Input* IScene::input_ = nullptr;
size_t IScene::playHandle_ = 0;

void IScene::StaticInitialize(Input* input)
{
	assert(input);
	input_ = input;
}

IScene::~IScene() {}

int IScene::GetSceneNo() { return sSceneNo_; }