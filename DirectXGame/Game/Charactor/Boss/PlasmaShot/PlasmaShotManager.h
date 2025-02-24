#pragma once
///---------------------------------------------------------------------------------------------
// 
// 電気玉攻撃の管理
// 
///---------------------------------------------------------------------------------------------

#include "PlasmaShot.h"
#include "ModelManager.h"
#include "Camera.h"

#include <array>

//電気玉攻撃マネージャクラス
class PlasmaShotManager {
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
	/// パーティクルの描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void DrawParticle(const DaiEngine::Camera& camera);
	/// <summary>
	/// 衝突時
	/// </summary>
	/// <param name="index">インデックス</param>
	void OnCollision(size_t index, DaiEngine::Collider* other);
	/// <summary>
	/// ターゲットセット
	/// </summary>
	/// <param name="target">ターゲット</param>
	void SetTarget(const Vector3* target);
	/// <summary>
	/// 攻撃開始
	/// </summary>
	void AttackStart();
	/// <summary>
	/// 攻撃に必要なデータの設定
	/// </summary>
	/// <param name="pos">発射座標</param>
	void SetAttackData(const Vector3& pos);
	/// <summary>
	/// 攻撃が終わった瞬間
	/// </summary>
	/// <returns>攻撃が終わった瞬間true、それ以外はfalse</returns>
	bool AttackFinish() const { return (!isAttack_ && preIsAttack_); }
	/// <summary>
	/// 攻撃中か
	/// </summary>
	/// <returns>攻撃中ならtrue、それ以外はfalse</returns>
	bool IsAttack()const { return isAttack_; }
	/// <summary>
	/// 生存しているか
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>存在していたらtrue、それ以外はfalse</returns>
	bool IsLife(uint32_t index) const { return plasmaShots_[index]->IsLife(); }
	/// <summary>
	/// 電気玉の総数取得
	/// </summary>
	/// <returns>電気玉の総数</returns>
	uint32_t GetPlasmaShotCount() const { return kPlasmaShotNum_; }

private:
	//電気玉の数
	static const uint32_t kPlasmaShotNum_ = 3;
	//電気玉の配列
	std::array<std::unique_ptr<PlasmaShot>, kPlasmaShotNum_> plasmaShots_;

	//攻撃フラグ
	bool isAttack_ = false;
	bool preIsAttack_ = false;

};

