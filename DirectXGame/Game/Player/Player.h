#pragma once
#include <optional>
#include "Audio.h"

#include "Engine/Input/input.h"
#include "ImGuiManager.h"
#include "Particle.h"

#include "Game/Character/Character.h"
enum class Behavior {
	kRoot,//通常
	kJump,//ジャンプ中
	kGrap,//つかみ
};
enum class GrapBehavior {
	kLeft,//左
	kRight//右
};
enum audioHundle {
	Rotation,
	GrapJump,
};

class Player :public Character
{
public:
	void Init(std::vector<Object3d*> models)override;
	void Update()override;
	void Draw(const Camera& camera)override;

	void SetViewProjection(const Camera* camera) {
		camera_ = camera;
	}
	void HitSango() {
		canGrap = true;
	}
	void hitBox(Vector3 colliderPos,Vector3 colliderSize);
	void ImGui();

#pragma region
	const WorldTransform& GetWorldTransform() {
		return world_;
	}
	GrapBehavior GetGrapBehavior()const {
		return GrapBehavior_;
	}
	Behavior GetBehavior()const {
		return behavior_;
	}
	bool GetFarstFlag()const {
		return farstFlag;
	}
	bool GetgrapJumpFlag()const {
		return grapJump;
	}
	bool GetCanGrapFlag()const {
		return canGrap;
	}
	bool GetP_RoringFlag()const {
		return P_RoringFlag;
	}
	bool GetP_AutoGrapFlag()const {
		return P_AutoGrapFlag;
	}
	bool GetMaxPower() {
		return maxPower_;
	}
#pragma endregion getter
#pragma region
	void SetSangoId(int sangoId) { 	sangoId_ = sangoId; }
	void SetSangoPos(Vector3 Pos) { grapPoint = Pos; }
	void SetPos(const Vector3& pos) { world_.translation_ = pos; }

	void SetSoundHundle(const std::vector<size_t> handle) { sounds_ = handle; }
	
	void SetRepopPos(const Vector3& pos) { repopPos_ = pos; }
#pragma endregion setter

private:

	Vector3 repopPos_ = {};

	//ビュープロジェクション
	const Camera* camera_;

	void QuaternionUpdate();
	void WorldUpdate();

#pragma region
	void Gravity();
	bool IsOnGraund = false;
#pragma endregion 重力
#pragma region
	//ふるまい
	Behavior behavior_ = Behavior::kRoot;
	//次のふるまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;
	//通常
	void BehaviorRootInit();
	void BehaviorRootUpdate();
	Vector3 move;
	//プレイヤーの移動
	float speed = 0.2f;
	//ジャンプ
	void BehaviorJumpInit();
	void BehaviorJumpUpdate();
	Vector3 jumpForce;
#pragma endregion Behavior
#pragma region
	//つかむ
		//ふるまい
	GrapBehavior GrapBehavior_ = GrapBehavior::kRight;
	//次のふるまいリクエスト
	std::optional<GrapBehavior> GrapBehaviorRequest_ = std::nullopt;
	void GrapInit();
	void GrapUpdate();
	void GrapSelectDirection();
	void GrapJumpLeftInitalize();
	void GrapJumpLeftUpdate();
	void GrapJumpRightInitalize();
	void GrapJumpRightUpdate();
	bool maxPower_ = false;
	Quaternion playerLock;
	bool canGrap = false;
	Vector3 grapPoint;
	Quaternion rotateQua;
	Quaternion directionQua_;
	bool grapJump = false;
	Vector3 grapJumpVec;
	float angle = 1.0f;
	const float kAngleMax = 0.5f;
	int JumpFlame;
	Vector3 moveVector;
	Vector3 sangoDirection_ = { 0.0f,0.0f,0.0f };
	const float kGravity = -0.7f;
	//当たり判定の履歴
	int sangoId_ = 0;
	int PreSangoId_ = -1;
	const float kDeletePreIdTime_ = 20.0f;
	float DeletePreIdTime_ = 0.0f;
	Quaternion PlayerDirectionQua_;
	bool secondJump = false;
	Vector3 secondJumpVec;
	//プレイヤージャンプ
	float jumpParam = 0.0f;
	float kjumpParam = 1.0f;
	float addJumpParam = 0.01f;
	float subJumpParam = 0.004f;
#pragma endregion GrapBehavior
#pragma region
	WorldTransform world_Arrow_;
	Quaternion ArrowQua_;
#pragma endregion 矢印
#pragma region
	void Move();
	Quaternion playerQua_;
	Quaternion moveQua_;
	float moveParam = 0.0f;
	Vector3 direction = { 0.0f,0.0f,0.0f };
	Vector3 tlanslatePre;
#pragma endregion 移動
	bool farstFlag = true;
#pragma region
	std::vector<size_t> sounds_;
	bool IsRoringSound = false;
	bool IsGrapjumpSound = false;
	int roringCount = 0;
	int roringMaxCount = 35;
#pragma endregion 音
#pragma region
	bool P_RoringFlag = false;
	bool P_AutoGrapFlag = false;
#pragma endregion パーティクル用フラグ

};