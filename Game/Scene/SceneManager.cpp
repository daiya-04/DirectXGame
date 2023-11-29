#include "SceneManager.h"

#include "SceneList.h"
#include "../Object/BlockManager.h"
#include "../Object/MapManager.h"

using enum IScene::SCENE;

SceneManager* SceneManager::GetInstace()
{
	static SceneManager instance;
	return &instance;
}

void SceneManager::Initialize()
{
	sceneArray_[kTITLE] = std::make_unique<TitleScene>();
	sceneArray_[kSELECT] = std::make_unique<SelectScene>();
	sceneArray_[kGAME] = std::make_unique<GameScene>();


	currentSceneNo_ = kTITLE;
	preSceneNo_ = kTITLE;

	for (size_t i = 0; i < IScene::kCountScene; i++)
	{
		sceneArray_[i]->Initialize();
	}

	BlockManager::GetInstance()->Initialize();
	MapManager::GetInstance()->Initialize();

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

void SceneManager::DrawParticle()
{
	sceneArray_[currentSceneNo_]->DrawParticle();
}

void SceneManager::ChegeScene(int num)
{
	assert(num < kCountScene);

	size_t selectSEHandle = Audio::GetInstance()->SoundLoadWave("select.wav");
	size_t selectSEPlayerHandle = Audio::GetInstance()->SoundPlayWave(selectSEHandle);

	preSceneNo_ = currentSceneNo_;
	currentSceneNo_ = num;
	sceneArray_[currentSceneNo_]->Reset();
}

void SceneManager::SelectStage(int num)
{
	preSceneNo_ = currentSceneNo_;
	currentSceneNo_ = kGAME;

	size_t selectSEHandle = Audio::GetInstance()->SoundLoadWave("select.wav");
	size_t selectSEPlayerHandle = Audio::GetInstance()->SoundPlayWave(selectSEHandle);

	sceneArray_[currentSceneNo_]->SetStageNum(num);
	sceneArray_[currentSceneNo_]->Reset();
}

void SceneManager::DebugGUI()
{
#ifdef _DEBUG





#endif // _DEBUG

}
