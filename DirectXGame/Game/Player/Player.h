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

class Player:public Character
{
public:
	void Init(std::vector<Object3d*> models)override;
	void Update()override;
	void Draw(const Camera& camera)override;

	void SetViewProjection(const Camera* camera) {
		camera_ = camera;
	}

	void ImGui();

#pragma region
	const WorldTransform& GetWorldTransform() {
		return world_;
	}
#pragma endregion getter

private:
	//ビュープロジェクション
	const Camera* camera_;


	void QuaternionUpdate();
	void WorldUpdate();


#pragma region
	Input* input = nullptr;
#pragma endregion 入力

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
	bool grapJump = false;
	uint32_t grapJumpAnime = 0;
	Vector3 grapJumpVec;
	Quaternion endVecQua;
	Quaternion beginVecQua;
	Quaternion lerpQua;
	float angle = 1.0f;
	float angleParam = 0.0f;
	Vector3 moveVector;
	float jumpPower = 0.0f;
#pragma endregion GrapBehavior

#pragma region
	void Move();
	Quaternion playerQua_;
	Quaternion moveQua_;
#pragma endregion 移動
};