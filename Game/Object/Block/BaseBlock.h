#pragma once

#include <vector>

#include "../IObject.h"

class BaseBlock : public IObject
{
public:

	enum class BlockType : size_t {
		kBlockStage,	// ステージに使う
		kBlockBarrier,	// 妨害に使う

		kCountofBlockType,
	};

protected:

	// ブロックのサイズ
	BlockType type_ = BlockType::kBlockStage;

	// マップチップの設定
	Vector3 mapPosition_ = { 0.0f,0.0f,0.0f };


public:

	virtual void Initialize() override;
	virtual void Initialize(const Vector3& position, BlockType type);

	virtual void Update() override;

	virtual void Draw() override;

};