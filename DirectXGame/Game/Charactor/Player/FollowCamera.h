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

//前方宣言
class LockOn;

//追従カメラクラス
class FollowCamera{
private:
	//カメラ
	DaiEngine::Camera camera_;
	//追従先
	const DaiEngine::WorldTransform* target_ = nullptr;
	//ロックオン
	const LockOn* lockOn_ = nullptr;

	//追従対象の残像座標
	Vector3 interTarget_{};

	float deltaTime_ = 1.0f / 60.0f;

	//通常の減衰率
	uint32_t baseDampingRate_ = 7;
	//ダッシュ時の減衰率
	uint32_t dashDampingRate_ = 5;
	//ズーム時の減衰率
	uint32_t zoomDampingRate_ = 15;
	//ダッシュ用フラグ
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
	float zoomSpeed_ = 0.1f;

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
	/// オフセット計算
	/// </summary>
	/// <returns>オフセットベクトル</returns>
	Vector3 OffsetCalc();
	/// <summary>
	/// 残像の計算
	/// </summary>
	void CalcInterTarget();
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
	/// ズームの設定
	/// </summary>
	/// <param name="isAttack"></param>
	void SetZoomFlag(bool isZoom) { isZoom_ = isZoom; }
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

