#pragma once

#include <vector>
#include <memory>

#include "../Math/Math.h"

#include "../Object/BaseBlock.h"
#include "../Object/MapManager.h"
#include "../Object/BlockManager.h"

class DebugStage
{
private:

	// 変わらないステージの情報
	MapManager::StageData kStageData_;
	
	// マップの管理マネージャー
	MapManager* mapManager_ = nullptr;
	BlockManager* blockManager_ = nullptr;

public:

	DebugStage();
	~DebugStage();

	// ステージ情報の読み込みを行う
	void Initialize(ViewProjection* view);
	// ステージ情報をセット
	void Reset();

	void Update();

	void Draw();

	void DebugGUI();

private:

	// ステージ読み込み
	void LoadStageData();

	// ステージ書き込み
	void SaveStageData();

	// ステージの情報を反映
	void ApplyStageData();

};