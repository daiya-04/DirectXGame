#pragma once
#include "IBossBehavior.h"

#include "Vec3.h"
#include <array>

class BossMove : public IBossBehavior {
public:

	BossMove(Boss* boss) : boss_(boss) {}

	~BossMove() = default;

	void Init() override;

	void Update() override;

private:
	/// <summary>
	/// 移動するエリアを決める
	/// </summary>
	void DecideMoveArea();

private:

	Boss* boss_ = nullptr;

	struct Area {
		Vector3 centerPos_;
		float width_;
		float depth_;
	};

	//エリアの分割数
	static const uint32_t kDivisionNum_ = 25;
	//エリアの配列
	std::array<Area, kDivisionNum_> areas_;
	//移動先のエリア情報
	Area moveArea_;
	//移動先の座標
	Vector3 movePos_;

	float param_ = 0.0f;
	Vector3 moveStartPos_;
	float moveSpeed_ = 0.01f;


};

