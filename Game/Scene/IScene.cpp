#include "IScene.h"

// タイトルシーンで初期化
int IScene::sSceneNo_ = kTITLE;

IScene::~IScene() {}

int IScene::GetSceneNo() { return sSceneNo_; }