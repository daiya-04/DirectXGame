#include "SceneManager.h"

#include "SceneList.h"
using enum IScene::SCENE;

SceneManager* SceneManager::GetInstace()
{
	static SceneManager instance;
	return &instance;
}

void SceneManager::Initialize()
{
	sceneArray_[kTITLE] = std::make_unique<TitleScene>();
	sceneArray_[kGAME] = std::make_unique<GameScene>();


	currentSceneNo_ = kTITLE;
	preSceneNo_ = kTITLE;

	for (size_t i = 0; i < IScene::kCountScene; i++)
	{
		sceneArray_[i]->Initialize();
	}

	//sceneArray_[kTITLE]->Initialize();
}

void SceneManager::Update()
{
	sceneArray_[currentSceneNo_]->Update();
}

void SceneManager::DrawModel()
{
	sceneArray_[currentSceneNo_]->DrawModel();
}

void SceneManager::DrawUI()
{
	sceneArray_[currentSceneNo_]->DrawUI();
}

void SceneManager::ChegeScene(int num)
{
	assert(num < kCountScene);

	preSceneNo_ = currentSceneNo_;
	currentSceneNo_ = num;
	sceneArray_[currentSceneNo_]->Reset();
}

void SceneManager::DebugGUI()
{
#ifdef _DEBUG





#endif // _DEBUG

}
