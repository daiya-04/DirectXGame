#include "DebugStage.h"

DebugStage::DebugStage()
{
}

DebugStage::~DebugStage()
{
}

void DebugStage::Initialize()
{
	input_ = Input::GetInstance();

	LoadStageData();
	ApplyStageData();
}

void DebugStage::Reset()
{
	//LoadStageData();
	currentData_ = kStageData_;
	// ステージの情報を反映
	ApplyStageData();
}

void DebugStage::Update()
{
}

void DebugStage::Draw()
{
}

void DebugStage::LoadStageData()
{
	// ステージの大きさを決める
	kStageSize_[0] = 5;
	kStageSize_[1] = 5;
	kStageSize_[2] = 5;

	kStageData_.data_.resize(kStageSize_[0]);
	for (size_t i = 0; i < kStageSize_[0]; i++)
	{
		kStageData_.data_[i].resize(kStageSize_[1]);
		for (size_t j = 0; j < kStageSize_[1]; j++)
		{
			kStageData_.data_[i][j].resize(kStageSize_[2]);
			for (size_t k = 0; k < kStageSize_[2]; k++)
			{
				//kStageData_.data_[i][j][k] = BaseBlock::Element::kNone;
			}
		}
	}

	SetElement(2, 1, 0, BaseBlock::Element::kPlayer);

}

void DebugStage::SaveStageData()
{

}

void DebugStage::ApplyStageData()
{
}

void DebugStage::SetElement(const Vector3& position, BaseBlock::Element element)
{
	//kStageData_.data_[(int)position.x][(int)position.y][(int)position.z] = element;
}

void DebugStage::SetElement(size_t x, size_t y, size_t z, BaseBlock::Element element)
{
	//kStageData_.data_[x][y][z] = element;
}

void DebugStage::OparationUpdate()
{
	// 実際にはカメラの方向によって回転させる
	if (input_->TriggerKey(DIK_A)) {
		playerMoveDirect_.x = -1.0f;
	}
	if (Vector3(0.0f, 0.0f, 0.0f) != CalcMove(&playerPosition_, &playerMoveDirect_)) {
		// うごける
	}


}

Vector3 DebugStage::CalcMove(Vector3* position, Vector3* direct)
{
	Vector3 zeroVector{ 0.0f,0.0f,0.0f };
	// 移動方向を決定
	Vector3 moveVelocity{ zeroVector };

	// 方向がカメラからの視点になってると思うので変換必須
	Vector3 normalDirect{ *direct };

	// 移動方向に変化が無かった場合
	if (*direct == zeroVector) {
		return zeroVector;
	}
	bool result = false;
	// 移動方向になにかあるか
	if (direct->x != 0.0f) {
		result = ScanDirectionX(position,&normalDirect);
	}
	else if (direct->y != 0.0f) {
		result = ScanDirectionY(position, &normalDirect);
	}
	else if (direct->z != 0.0f) {
		result = ScanDirectionZ(position, &normalDirect);
	}

	// 範囲外に出てしまった時
	// ありえない数値入れて範囲外に出す
	if (0) {
		moveVelocity = *direct * 50.0f;
	}

	return moveVelocity;
}

bool DebugStage::ScanDirectionX(Vector3* position, Vector3* direct)
{
	// 範囲外になるまで走査
	size_t xIndex = (size_t)position->x;
	// 0 または サイズ
	// 一応マップ最大まで
	for (size_t i = 0; i < kStageSize_[0]; i++)
	{
		// 方向を使う
		int index = (int)xIndex + (int)i * (int)direct->x;
		// 範囲内なら
		if (0 < index && index < kStageSize_[0]) {

		}
		else {
			return false;
		}
	}
	return false;
}

bool DebugStage::ScanDirectionY(Vector3* position, Vector3* direct)
{
	return false;
}

bool DebugStage::ScanDirectionZ(Vector3* position, Vector3* direct)
{
	return false;
}

BaseBlock::Element DebugStage::GetElementPosition(const Vector3& position)
{
	//return currentData_.data_[(size_t)position.x][(size_t)position.y][(size_t)position.z];
	return BaseBlock::Element::kNone;
}

void DebugStage::GetPlayerPosition()
{
	for (size_t i = 0; i < kStageSize_[0]; i++)
	{
		for (size_t j = 0; j < kStageSize_[1]; j++)
		{
			for (size_t k = 0; k < kStageSize_[2]; k++)
			{
				/*
				if (currentData_.data_[i][j][k] == BaseBlock::Element::kPlayer) {
					playerPosition_ = { (float)i,(float)j,(float)k };
					return;
				}*/
			}
		}
	}
}
