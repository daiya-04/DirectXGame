#pragma once


#include <memory>
#include "IScene.h"

class SceneManager  final
{
private: // プライベート変数

	std::unique_ptr<IScene> sceneArray_[static_cast<const size_t>(IScene::kCountScene)];

	// どのシーンを呼び出すかを管理
	int currentSceneNo_;
	int preSceneNo_;

public: // パブリック関数	

	static SceneManager* GetInstace();

	void Initialize();

	void Update();
	
	void DrawModel();

	void DrawUI();

	void DrawParticle();

	void ChegeScene(int num);
	// ゲームシーンへ
	void SelectStage(int num);

	void DebugGUI();

private:

	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(const SceneManager& obj) = delete;
	const SceneManager& operator=(const SceneManager& obj) = delete;
};