#pragma once

#include <vector>
#include <memory>

#include "../Math/Math.h"

#include "../Object/BaseBlock.h"
#include "../Object/MapManager.h"
#include "../Object/BlockManager.h"

#include "../../GlobalVariables.h"

class Stage
{
protected:

	// 変わらないステージの情報
	MapManager::StageData kStageData_;
	
	// マップの管理マネージャー
	MapManager* mapManager_ = nullptr;
	BlockManager* blockManager_ = nullptr;

	GlobalVariables* variables_ = nullptr;
	int stageNum_ = 0;

	MapManager::MoveDirect cameraDirect_ = MapManager::MoveDirect::dFRONT;

public:

	Stage();
	virtual ~Stage();

	// ステージ情報の読み込みを行う
	virtual void Initialize(ViewProjection* view);
	// ステージ情報をセット
	virtual void Reset();

	virtual void Update();

	virtual void Draw();

	virtual void DebugGUI();

	void SetStageNum(int num) { stageNum_ = num; }

	size_t GetStageSize() const { return kStageData_.kMaxStageSize_.x; }

	void SetCameraDirection(MapManager::MoveDirect direct) { cameraDirect_ = direct; }

protected:

	// ステージ読み込み
	virtual void LoadStageData();

	// ステージの情報を反映
	virtual void ApplyStageData();

};