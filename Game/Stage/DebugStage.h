#pragma once

#include <vector>
#include <memory>

#include "../Math/Math.h"
#include "../../Input.h"

#include "../Object/StageObject.h"

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
	
	// 基本的に同時に座標は動かす
	Vector3 playerPosition_ = { 0.0f,0.0f,0.0f };
	Vector3 playerMoveDirect_ = { 0.0f,0.0f,0.0f };


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

	void SetElement(const Vector3& position, BaseBlock::Element element);
	void SetElement(size_t x, size_t y, size_t z, BaseBlock::Element element);

	// 移動を検知する
	void OparationUpdate();

	// 移動方向を計算する
	// 移動しない時はゼロベクトル
	Vector3 CalcMove(Vector3* position, Vector3* direct);

	// 一方向を走査する
	bool ScanDirectionX(Vector3* position, Vector3* direct);
	bool ScanDirectionY(Vector3* position, Vector3* direct);
	bool ScanDirectionZ(Vector3* position, Vector3* direct);

	BaseBlock::Element GetElementPosition(const Vector3& position);

	// ポジションを取得
	// 重いと思うので極力つかわない
	void GetPlayerPosition();

};