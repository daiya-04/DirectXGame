#pragma once

#include <vector>
#include "../Math/Math.h"

class DebugStage
{
private:

	struct StageData
	{
		std::vector<Vector3> blockData_;
		std::vector<Vector3> darumaData_;
		Vector3 playerData_;
	};

	// 変わらないステージの情報
	StageData kStageData_;
	// 現在のステージの情報
	StageData currentData_;

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

};