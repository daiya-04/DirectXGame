#pragma once
///---------------------------------------------------------------------------------------------
// 
// 追従カメラ
// プレイヤーに追従するカメラ
// 
///---------------------------------------------------------------------------------------------

#include "Camera.h"
#include "WorldTransform.h"
#include "Vec3.h"
#include <optional>
#include <map>
#include <functional>

//前方宣言
class LockOn;

//追従カメラクラス
class FollowCamera{
public:

	enum class Mode {
		kNormal,
		kAttack,
	};

private:

	Mode mode_ = Mode::kNormal;
	std::optional<Mode> modeRequest_ = Mode::kAttack;

	std::map<Mode, std::function<void()>> modeInitTable_{
		{Mode::kNormal, [this]() { NormalInit(); }},
		{Mode::kAttack, [this]() { AttackInit(); }},
	};

	std::map<Mode, std::function<void()>> modeUpdateTable_{
		{Mode::kNormal, [this]() { NormalUpdate(); }},
		{Mode::kAttack, [this]() { AttackUpdate(); }},
	};

private:

	void NormalInit();
	void NormalUpdate();

	void AttackInit();
	void AttackUpdate();


private:
	//カメラ
	DaiEngine::Camera camera_;
	//追従先
	const DaiEngine::WorldTransform* target_ = nullptr;
	//ロックオン
	const LockOn* lockOn_ = nullptr;

	//追従対象の残像座標
	Vector3 interTarget_{};

	//遅延中の時間
	float delayParam_ = 0.0f;
	//遅延する時間
	uint32_t delayTime_ = 30;
	uint32_t dashDelayTime_ = 10;

	float deltaTime_ = 1.0f / 60.0f;

	uint32_t baseDampingRate_ = 8;
	uint32_t dashDampingRate_ = 2;
	uint32_t zoomDampingRate_ = 15;

	bool isDash_ = false;

	Vector3 rotation_ = {};
	Matrix4x4 rotateMat_ = MakeIdentity44();
	//オフセットのベース
	Vector3 baseOffset_= { 0.0f,3.0f,-8.0f };
	//攻撃時のオフセット
	Vector3 attackOffset_ = { 1.5f,1.5f,-3.0f };
	//ロックオン先との距離の半分
	Vector3 halfPos_ = {};
	//ズーム用フラグ
	bool isZoom_ = false;

	float zoomParam_ = 0.0f;
	float zoomSpeed_ = 1.0f / 20.0f;


	struct WorkAttack {
		Vector3 startPos_;
		Vector3 endPos_;
		Vector3 startRotate_;
		Vector3 endRotate_;
		float param_;
		float speed_;
	};

	WorkAttack workAttack_{};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// リセット
	/// </summary>
	void Reset();
	/// <summary>
	/// オフセット計算
	/// </summary>
	/// <returns>オフセットベクトル</returns>
	Vector3 OffsetCalc();
	/// <summary>
	/// ターゲットセット
	/// </summary>
	/// <param name="target">追従先のワールドトランスフォーム</param>
	void SetTarget(const DaiEngine::WorldTransform* target);
	/// <summary>
	/// ロックオンセット
	/// </summary>
	/// <param name="lockOn"></param>
	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }
	/// <summary>
	/// カメラのモード変更
	/// </summary>
	/// <param name="mode"></param>
	void ModeChange(Mode mode) { modeRequest_ = mode; }
	/// <summary>
	/// ズームの設定
	/// </summary>
	/// <param name="isAttack"></param>
	void SetZoom(bool isZoom) { isZoom_ = isZoom; }
	/// <summary>
	/// ダッシュの設定
	/// </summary>
	/// <param name="isDash"></param>
	void SetDashFlag(bool isDash) { isDash_ = isDash; }
	/// <summary>
	/// カメラ情報取得
	/// </summary>
	/// <returns>カメラ</returns>
	DaiEngine::Camera& GetCamera() { return camera_; }
	/// <summary>
	/// 回転行列の取得
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetRotateMat() const { return rotateMat_; }
	
	

};

