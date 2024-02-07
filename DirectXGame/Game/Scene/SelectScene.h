#pragma once
#include "IScene.h"
#include <memory>
#include <list>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"
#include"Quaternion.h"


class SelectScene : public IScene{
public:

	void Init()override;

	void Update()override;	

	void DrawBackGround()override;

	void DrawModel()override;

	void DrawParticleModel()override;

	void DrawParticle()override;

	void DrawUI()override;

	void DebugGUI()override;

	void UIInit();

	~SelectScene()override;

	static int GetSelectNumber() { return selectNum_; }

	static void SetClearFlag(int x) { isStageClear_[x] = true; }

	static void ClearFlagReset() {
		for (int i = 0; i < maxStage_-1; i++){
			isStageClear_[i] = false;
		}
	}

private:
	void SelectStage();

	void EnterTheStage();

private:
	//ステージセレクト用の変数
	static int selectNum_;
	int oldSelectNum_ = 0;
	static const int maxStage_ = 4;
	//補完の変数群
	const float addEase_ = 0.04f;
	float easeT_ = 0.0f;

	const float addRotateEase_ = 0.2f;
	float easeRotateT_ = 1.0f;

	const float movePos_ = 7.0f;

	float startPos_ = 0.0f;
	float endPos_ = 3.14f;

	Vector3 startPlayerPos_ = { 0.0f ,0.0f,0.0f };
	Vector3 endPlayerPos_ = { 3.14f ,0.0f ,0.0f };

	float startRotate_ = 3.14f;
	float endRotate_ = 3.14f;
	//振り向いた後ちょっと待つためのタイマー
	int cooltime_ = 30;

	Camera camera_;
	//モデルデータの入れ物
	Model* Model_ = 0;
	Model* seaHorseModel_ = 0;
	Model* floorModel_ = 0;
	Model* rockModel_ = 0;
	Model* stageSelectModel_ = 0;
	Model* skyModel_ = 0;
	Model* loadModel_ = 0;

	//オブジェクト
	std::unique_ptr<Object3d> obj_[maxStage_];	

	std::unique_ptr<Object3d> loadObj_;

	std::unique_ptr<Object3d> sLoadObj_[maxStage_ - 1];

	std::unique_ptr<Object3d> floorObj_;
	std::unique_ptr<Object3d> rockObj_[maxStage_];
	std::unique_ptr<Object3d> stageSelectObj_[maxStage_];

	std::unique_ptr<Object3d> playerObj_;

	std::unique_ptr<Object3d> skyDomeObj_;

	//ワールドトランスフォーム
	WorldTransform objWT_[maxStage_];	

	WorldTransform loadWT_;
	WorldTransform sloadWT_[maxStage_ - 1];

	WorldTransform floorWT_;
	WorldTransform rockWT_[maxStage_];
	WorldTransform stageSelectWT_[maxStage_];

	WorldTransform playerWT_;
	WorldTransform seaHorseWT_;

	WorldTransform skyDomeWT_;

	//自キャラの回転に使う予定だったものたち
	Matrix4x4 rotateMat_;

	Vector3 from_ = { 0.0f,0.0f,1.0f };
	Vector3 rotate_ = { 0.0f,0.0f,1.0f };

	Quaternion quater_;

	Input* input_ = nullptr;

	bool isSceneNext_ = false;

	bool isSelectEnd_ = false;

	static bool isStageClear_[maxStage_];


	//UI
	std::unique_ptr<Sprite> APress_;
	
	std::unique_ptr<Sprite> stick_;

	std::unique_ptr<Sprite> moveText_;

	std::unique_ptr<Sprite> stageName_;

	WorldTransform pressTrans_;

	WorldTransform stickTrans_;

	WorldTransform moveTrans_;

	WorldTransform stageNameWT_;

	uint32_t stageT_ = 0;
	uint32_t stage1_ = 0;
	uint32_t stage2_ = 0;
	uint32_t stage3_ = 0;

	int UITimer_ = 0;
};

