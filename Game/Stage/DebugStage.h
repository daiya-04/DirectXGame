#pragma once


#include "Stage.h"

class DebugStage : public Stage
{
public:

	DebugStage();
	~DebugStage() override;

	// ステージ情報の読み込みを行う
	void Initialize(ViewProjection* view) override;
	// ステージ情報をセット
	void Reset() override;

	void Update() override;

	void Draw() override;

	void DebugGUI() override;

private:

	// ステージ読み込み
	void LoadStageData() override;

	// ステージ書き込み
	void SaveStageData();

	// ステージの情報を反映
	void ApplyStageData() override;

};