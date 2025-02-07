#pragma once
///---------------------------------------------------------------------------------------------
// 
// 火の玉攻撃の管理
// 
///---------------------------------------------------------------------------------------------

#include "ElementBall.h"
#include "BurnScar.h"
#include "ModelManager.h"
#include <Camera.h>

#include <array>

//火の玉攻撃マネージャクラス
class ElementBallManager {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	void Init(const std::shared_ptr<DaiEngine::Model>& model);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const DaiEngine::Camera& camera);
	/// <summary>
	/// パーティクル描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void DrawParticle(const DaiEngine::Camera& camera);
	/// <summary>
	/// 焼け跡描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void DrawBurnScars(const DaiEngine::Camera& camera);
	/// <summary>
	/// ターゲットセット
	/// </summary>
	/// <param name="target">ターゲットのワールドトランスフォーム</param>
	void SetTartget(const Vector3* target);
	/// <summary>
	/// データセット
	/// </summary>
	/// <param name="pos">発射位置</param>
	void SetAttackData(const Vector3& pos);
	/// <summary>
	/// 攻撃開始
	/// </summary>
	void AttackStart();
	/// <summary>
	/// 攻撃終了の瞬間
	/// </summary>
	/// <returns>攻撃が終わった瞬間ならtrue、それ以外はfalse</returns>
	bool AttackFinish() const { return (!isAttack_ && preIsAttack_); }
	/// <summary>
	/// 攻撃中か
	/// </summary>
	/// <returns>攻撃中ならtrue、それ以外はfalse</returns>
	bool IsAttack() const { return isAttack_; }
	/// <summary>
	/// 発射開始の瞬間
	/// </summary>
	/// <returns>発射が始まった瞬間ならtrue、それ以外はfalse</returns>
	bool ShotStart() const { return (!isShot_ && preIsShot_); }
	/// <summary>
	/// 存在しているか
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>存在していればtrue、それ以外はfalse</returns>
	bool IsLife(uint32_t index)const { return elementBalls_[index]->IsLife(); }
	/// <summary>
	/// 火の玉の数取得
	/// </summary>
	/// <returns>火の玉の数</returns>
	uint32_t GetElementballCount() const { return kElementBallNum_; }

	Vector3 GetWorldPos(uint32_t index) { return elementBalls_[index]->GetWorldPos(); }
	

private:
	//玉の総数
	static const uint32_t kElementBallNum_ = 4;
	//火の弾の配列
	std::array<std::unique_ptr<ElementBall>, kElementBallNum_> elementBalls_;
	//焼け跡の配列
	std::array<std::unique_ptr<BurnScar>, kElementBallNum_> burnScareses_;

	//攻撃フラグ
	bool isAttack_ = false;
	bool preIsAttack_ = false;

	//発射フラグ
	bool isShot_ = false;
	bool preIsShot_ = false;

};

