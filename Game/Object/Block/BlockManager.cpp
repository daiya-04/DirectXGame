#include "BlockManager.h"

StageBlock::StageBlock()
{
}

StageBlock::~StageBlock()
{
}


void StageBlock::Initialize(ViewProjection* vp)
{
	viewProjection_ = vp;
	blocks_.clear();
}

void StageBlock::Update()
{
	for (std::unique_ptr<BaseBlock>& block : blocks_) {
		block->Update();
	}
}

void StageBlock::Draw()
{
	for (std::unique_ptr<BaseBlock>& block : blocks_) {
		block->Draw();
	}
}

void StageBlock::CreateBlock(const Vector3& mapPosition, BaseBlock::BlockType type)
{
	BaseBlock* block = new BaseBlock();
	// 三次元マップ上の位置を設定
	kBlockSize_ = mapPosition;

	// モデル用に作成
	WorldTransform wt;
	// 描画位置をマップの中心に合わせる
	wt.translation_ = { kBlockSize_.x / 2.0f,kBlockSize_.y / 2.0f,kBlockSize_.z / 2.0f };
	// マップの場所に合わせる
	wt.translation_.x = mapPosition.x * kBlockSize_.x;
	wt.translation_.y = mapPosition.y * kBlockSize_.y;
	wt.translation_.z = mapPosition.z * kBlockSize_.z;
	block->Initialize(mapPosition, type);
	block->AddWorldTransform(wt, true);
	block->UpdateMatrixs();

	blocks_.emplace_back(block);
}
