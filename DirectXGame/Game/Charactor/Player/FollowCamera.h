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

//追従カメラクラス
class FollowCamera{
private:
	//カメラ
	Camera camera_;
	//追従先
	const WorldTransform* target_ = nullptr;

	//追従対象の残像座標
	Vector3 interTarget_{};

	//遅延中の時間
	float delayParam_ = 0.0f;
	//遅延する時間
	uint32_t delayTime_ = 15;

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
	Vector3 OffsetCalc() const;
	/// <summary>
	/// ターゲットセット
	/// </summary>
	/// <param name="target">追従先のワールドトランスフォーム</param>
	void SetTarget(const WorldTransform* target);
	/// <summary>
	/// カメラ情報取得
	/// </summary>
	/// <returns>カメラ</returns>
	Camera& GetCamera() { return camera_; }

};

