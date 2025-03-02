#include "Framework.h"

#include "ImGuiManager.h"
#include "Input.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "DirectionalLight.h"
#include "Line.h"
#include "GlobalVariables.h"
#include "Audio.h"
#include "DXCompiler.h"
#include "PipelineManager.h"
#include "RandomEngine.h"

namespace DaiEngine {

	void DSFramework::Init() {

		WinApp::GetInstance()->CreateGameWindow(L"魔法師の戦い");


		DirectXCommon::GetInstance()->Initialize();
		ImGuiManager::GetInstance()->Initialize();

		DXCompiler::GetInstance()->Init();

		Input::GetInstance()->Initialize();
		Audio::GetInstance()->Initialize();

		TextureManager::GetInstance()->Initialize();
		TextureManager::GetInstance()->LoadAllFile();

		RandomEngine::GetInstance();

		PipelineManager::GetInstance()->Init();

		Line::Init();

		GlobalVariables::GetInstance()->LoadFiles();

		DirectionalLight::GetInstance()->Init();

	}

	void DSFramework::Update() {

		if (WinApp::GetInstance()->ProcessMessage()) { endRequest_ = true; }


		ImGuiManager::GetInstance()->Begin();

		Input::GetInstance()->Update();
		Audio::GetInstance()->Update();

		GlobalVariables::GetInstance()->Update();

		DirectionalLight::GetInstance()->Update();

		SceneManager::GetInstance()->Update();

	}

	void DSFramework::Finalize() {

		ImGuiManager::GetInstance()->Finalize();
		WinApp::GetInstance()->TerminateGameWindow();

	}

	void DSFramework::Run() {

		//初期化
		Init();

		//ゲームループ
		while (true) {

			//更新
			Update();

			//終了リクエストが来たら抜ける
			if (IsEndRequest()) { break; }

			//描画
			Draw();

		}

		//ゲームの終了
		Finalize();
	}
}
