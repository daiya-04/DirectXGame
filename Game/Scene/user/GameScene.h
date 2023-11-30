#pragma once

#include "../IScene.h"

#include "../../Stage/Stage.h"
#include "../../../Particle.h"
#include "../../../StageCamera.h"
#ifdef _DEBUG

#include "../../Stage/DebugStage.h"

#endif // _DEBUG


class GameScene final : public IScene
{
public:
	// 初期化
	void Initialize() override;
	// シーン切り替え時の処理
	void Reset() override;

	// 更新
	void Update() override;

	// モデルの描画
	void DrawModel() override;
	// UI の描画
	void DrawUI() override;
	// Particle の描画
	void DrawParticle() override;

	// デストラクタ
	~GameScene() override;

	void DebugGUI() override;

	void SetStageNum(int num) override { stageNum_ = num; }

private: // プライベート関数

	MapManager::MoveDirect CameraDirection();


private: // プライベート変数


	// ゲームシーン内でデータを保存する
	ViewProjection* maingCamera_ = nullptr;;

	int stageNum_ = 0;

	// ステージを持たせる
	std::unique_ptr<Stage> currentStage_;

	//ステージカメラ
	std::unique_ptr<StageCamera> stageCamera_;

	// 原点へのオフセット
	/*Vector3 kOriginOffset_ = { 0.0f,0.0f,0.0f };
	Vector3 kOriginAngle = { 0.0f,0.0f,0.0f, };*/

	//Particle
	std::unique_ptr<Particle> clearParticle_;
	std::list<Particle::ParticleData> clearParticles_;

	// ゲームオーバー
	int32_t cGameOverStagingTime_ = 120;
	int32_t gameOverStagingTime_ = 0;

	//UI
	std::unique_ptr<Sprite> movePlayerKey_;
	std::unique_ptr<Sprite> moveCameraKey_;
	std::unique_ptr<Sprite> clearCondition_;
	std::unique_ptr<Sprite> playerMode_;
	std::unique_ptr<Sprite> spaceKey_;
	std::unique_ptr<Sprite> stageName_;

	std::array<uint32_t, 2> clearConditionHandle{};
	std::array<uint32_t, 2> playerMadeHandle{};
	std::array<uint32_t, 8> stageNameHandle{};

	bool isShot_ = false;

};