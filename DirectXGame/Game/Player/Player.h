#pragma once
#include <optional>

#include "Engine/Input/input.h"
#include "ImGuiManager.h"

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

	void HitFloor(float PosY) {
		world_.translation_.y = PosY;
		world_.UpdateMatrixQua(moveQua_.MakeRotateMatrix());
		IsOnGraund = true;
	}
	void HitSango(Vector3 Pos) {
		grapPoint = Pos;
		canGrap = true;
	}
	void hitBox() {

	}
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
#pragma endregion getter
#pragma region
	void SetSangoId(int sangoId) { 	sangoId_ = sangoId; }
#pragma endregion setter

private:
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
	float speed = 0.5f;
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
	void GrapJumpLeftInitalize();
	void GrapJumpLeftUpdate();
	void GrapJumpRightInitalize();
	void GrapJumpRightUpdate();
	Quaternion playerLock;
	bool canGrap = false;
	Vector3 grapPoint;
	Quaternion rotateQua;
	Quaternion directionQua_;
	bool grapJump = false;
	Vector3 grapJumpVec;
	Quaternion endVecQua;
	Quaternion beginVecQua;
	Quaternion lerpQua;
	float angle = 1.0f;
	const float kAngleMax = 0.5f;
	int JumpFlame;
	float angleParam = 0.0f;
	float kParam = 0.005f;
	Vector3 moveVector;
	Vector3 sangoDirection_ = { 0.0f,0.0f,0.0f };
	float jumpParam = 0.8f;
	const float kGravity = -0.7f;
	//当たり判定の履歴
	int sangoId_ = 0;
	int PreSangoId_ = -1;
	const float kDeletePreIdTime_ = 20.0f;
	float DeletePreIdTime_ = 0.0f;
	Quaternion PlayerDirectionQua_;
	bool secondJump = false;
	Vector3 secondJumpVec;
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
#pragma endregion 移動
};