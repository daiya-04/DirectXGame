#pragma once

#include <vector>
#include <memory>

#include "../Math/Math.h"
#include "../../Input.h"

#include "../Object/BaseBlock.h"

class DebugStage
{
private:


	struct StageData
	{
		std::vector<std::vector<std::vector< BaseBlock*>>> data_;
	};
	// 外部ファイル、または読み込み時に決定
	size_t kStageSize_[3];

	// 変わらないステージの情報
	StageData kStageData_;
	// 現在のステージの情報
	StageData currentData_;

	Input* input_ = nullptr;
	
public:

	DebugStage();
	~DebugStage();

	// ステージ情報の読み込みを行う
	void Initialize();
	// ステージ情報をセット
	void Reset();

	void Update();

	void Draw();

private:

	// ステージ読み込み
	void LoadStageData();

	// ステージ書き込み
	void SaveStageData();

	// ステージの情報を反映
	void ApplyStageData();

};