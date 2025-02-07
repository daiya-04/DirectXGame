#pragma once
#include <d3d12.h>
#include "IScene.h"
#include "AbstractSceneFactory.h"
#include "Sprite.h"
#include <memory>

namespace DaiEngine {
	class SceneManager final {
	private:

		AbstractSceneFactory* sceneFactory_ = nullptr;

		std::unique_ptr<IScene> scene_;
		std::unique_ptr<IScene> nextScene_;

		//フェード演出用
		uint32_t fadeTex_ = 0;
		std::unique_ptr<Sprite> fade_;
		float alpha_ = 0.0f;

	public:

		static SceneManager* GetInstance();

		void Init();

		void Update();

		void Draw(ID3D12GraphicsCommandList* commandList);
		/// <summary>
		/// シーン切り替え
		/// </summary>
		/// <param name="sceneName">シーンの名前</param>
		void ChangeScene(const std::string& sceneName);

		void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

	private:

		SceneManager() = default;
		~SceneManager() = default;
		SceneManager(const SceneManager&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;

	};
}
