#pragma once

#include <vector>
#include <memory>

#include "BaseBlock.h"

/// <summary>
/// シングルトンパターンにしたい
/// モデルの切り替えをどうするか悩み中
/// </summary>
class BlockManager
{
private:

	typedef std::unique_ptr<BaseBlock> pBlock;

	std::vector<std::vector<std::vector<pBlock>>> currentData_;

public:

	void Initialize();

	void Update();

	void Draw();

	// ステージ情報を設定する関数


	// タイプによってモデルを設定する関数
	void SetModel(BaseBlock::Element elment);

};