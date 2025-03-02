#include "BossMove.h"

#include "Boss.h"
#include "Hit.h"
#include "RandomEngine.h"
#include "Easing.h"
#include <cassert>

void BossMove::Init() {

	assert(boss_);
	///エリア情報初期化
	for (size_t i = 0; i < 5; i++) {
		for (size_t j = 0; j < 5; j++) {
			areas_[j * 5 + i].centerPos_ = { 10.0f * (static_cast<float>(i) - 2.0f), 0.0f, 10.0f * (static_cast<float>(j) - 2.0f) };
			areas_[j * 5 + i].width_ = 10.0f;
			areas_[j * 5 + i].depth_ = 10.0f;
		}
	}
	///

	//移動先のエリア決定
	DecideMoveArea();

	movePos_.x = moveArea_.centerPos_.x + RandomEngine::GetFloatRandom(-moveArea_.width_ / 2.0f, moveArea_.width_ / 2.0f);
	movePos_.y = boss_->GetCenterPos().y;
	movePos_.z = moveArea_.centerPos_.z + RandomEngine::GetFloatRandom(-moveArea_.depth_ / 2.0f, moveArea_.depth_ / 2.0f);

	moveStartPos_ = boss_->GetCenterPos();

}

void BossMove::Update() {

	param_ += moveSpeed_;
	param_ = std::clamp(param_, 0.0f, 1.0f);

	float T = Easing::easeInOutCubic(param_);
	boss_->GetObj()->worldTransform_.translation_ = Lerp(T, moveStartPos_, movePos_);

	if (param_ >= 1.0f) {
		boss_->LookAtTarget();
		boss_->ChangeBehavior("Idle");
	}

}

void BossMove::DecideMoveArea() {
	//プレイヤー中心の一定範囲の円
	Shapes::Circle pCircle = {
		{boss_->GetTarget()->GetWorldPos().x,boss_->GetTarget()->GetWorldPos().z},
		23.0f
	};
	//移動できるエリアの番号の配列
	std::vector<size_t> indexList;
	//移動できるエリアを探す
	for (size_t index = 0; index < areas_.size(); index++) {
		//横幅半分
		float hWidth = areas_[index].width_ / 2.0f;
		//縦幅半分
		float hHeight= areas_[index].depth_ / 2.0f;
		//エリア情報から四角を作る
		Shapes::RectAngle rect = {
			areas_[index].centerPos_.x - hWidth, //左
			areas_[index].centerPos_.x + hWidth, //右
			areas_[index].centerPos_.z + hHeight, //上
			areas_[index].centerPos_.z - hHeight, //下
		};
		//範囲外かどうかチェック
		if (!IsCollision(pCircle, rect)) {
			indexList.push_back(index);
		}
	}
	//移動するエリアの番号をランダムで取り出す
	size_t areaNum = indexList[static_cast<size_t>(RandomEngine::GetIntRandom(0, static_cast<int>(indexList.size() - 1)))];
	moveArea_ = areas_[areaNum];
	
}
