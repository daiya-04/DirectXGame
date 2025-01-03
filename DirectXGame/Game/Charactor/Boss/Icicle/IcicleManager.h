#pragma once
///---------------------------------------------------------------------------------------------
// 
// つらら攻撃の管理
// 
///---------------------------------------------------------------------------------------------

#include "Icicle.h"
#include "ModelManager.h"
#include "Camera.h"
#include "IceScar.h"

#include <array>

//つらら攻撃マネージャクラス
class IcicleManager {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	void Init(const std::shared_ptr<Model>& model);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera& camera);
	/// <summary>
	/// パーティクル描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void DrawParticle(const Camera& camera);
	/// <summary>
	/// 霜描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void DrawScar(const Camera& camera);
	/// <summary>
	/// 衝突時
	/// </summary>
	/// <param name="index">インデックス</param>
	void OnCollision(uint32_t index);
	/// <summary>
	/// ターゲットセット
	/// </summary>
	/// <param name="target">ターゲットのワールドトランスフォーム発射座標</param>
	void SetTarget(const WorldTransform* target);
	/// <summary>
	/// 攻撃開始
	/// </summary>
	void AttackStart();
	/// <summary>
	/// 攻撃に必要なデータの設定
	/// </summary>
	/// <param name="pos">発射座標</param>
	/// <param name="direction">方向</param>
	void SetAttackData(const Vector3& pos, const Vector3& direction);
	/// <summary>
	/// 攻撃終了の瞬間
	/// </summary>
	/// <returns>攻撃が終わった瞬間true、それ以外false</returns>
	bool AttackFinish() const {return (!isAttack_ && preIsAttack_);}
	/// <summary>
	/// 攻撃中か
	/// </summary>
	/// <returns>攻撃中ならtrue、それ以外はfalse</returns>
	bool IsAttack()const { return isAttack_; }
	/// <summary>
	/// 生存しているか
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>存在していればtrue、それ以外false</returns>
	bool IsLife(uint32_t index) { return icicles_[index]->IsLife(); }
	/// <summary>
	/// コライダー取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>コライダー</returns>
	Shapes::Sphere GetCollider(uint32_t index) { return icicles_[index]->GetCollider(); }
	/// <summary>
	/// つららの総数
	/// </summary>
	/// <returns>つららの数</returns>
	uint32_t GetIcicleCount() const { return kIcicleNum_; }

	Vector3 GetWorldPos(uint32_t index) { return icicles_[index]->GetWorldPos(); }

private:
	//つららの数
	static const uint32_t kIcicleNum_ = 4;
	//つららの配列
	std::array<std::unique_ptr<Icicle>, kIcicleNum_> icicles_;
	//霜の配列
	std::array<std::unique_ptr<IceScar>, kIcicleNum_> iceScars_;

	//攻撃フラグ
	bool isAttack_ = false;
	bool preIsAttack_ = false;

};
