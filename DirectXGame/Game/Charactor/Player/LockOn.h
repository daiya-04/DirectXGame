#pragma once
#include "Vec3.h"

class Boss;

/// <summary>
/// ロックオンクラス
/// </summary>
class LockOn {
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
	/// ターゲットの設定(ロックオン開始)
	/// </summary>
	/// <param name="target"></param>
	void SetTarget(const Boss* target) { target_ = target; }
	/// <summary>
	/// ロックオン解除
	/// </summary>
	void LockOff() { target_ = nullptr; }
	/// <summary>
	/// ロックオン中か？
	/// </summary>
	/// <returns></returns>
	bool ExistTarget() const { return target_ ? true : false; }
	/// <summary>
	/// ターゲット座標取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetTargetPos() const;

private:

	//ロックオン対象
	const Boss* target_ = nullptr;

};

