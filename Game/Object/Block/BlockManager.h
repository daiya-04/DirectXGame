#pragma once

#include <vector>
#include <memory>

#include "../Block/BaseBlock.h"

class StageBlock
{
private:
	// ブロック共通のサイズ
	Vector3 kBlockSize_ = { 1.0f,1.0f,1.0f };

	std::vector<std::unique_ptr<BaseBlock>> blocks_;

	// 使わないかも
	ViewProjection* viewProjection_ = nullptr;

public:

	StageBlock();
	~StageBlock();

	void Initialize(ViewProjection* vp);

	void Update();

	void Draw();

	void CreateBlock(const Vector3& mapPosition, BaseBlock::BlockType type);

};