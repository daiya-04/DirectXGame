#pragma once

#include <vector>

#include "BaseBlock.h"
#include "../../Input.h"

/// <summary>
/// シングルトンパターンにしたい
/// </summary>
class MapManager
{
public:

	template<typename Ty>
	using StageArray = std::vector<std::vector<std::vector<Ty>>>;

	struct StageData
	{
		StageArray<BaseBlock::Element> array_;
	};

	// 移動方向
	enum MoveDirect : size_t
	{
		dNONE,
		dFRONT,
		dBACK,
		dRIGHT,
		dLEFT,
		dDOWN,
	};

private:

	StageData preData_;
	StageData currentData_;

	Input* input_ = nullptr;

	BaseBlock::StageVector playerPosition_;

public:

	void Initialize();

	void Update();

	void Draw();

	void DebugGUI();

	// ステージ情報を設定する関数
	void SetStageData(const StageArray<BaseBlock::Element>& data);

private:

	void GetOperate();

	void GetPlayerPosition();

	void MoveStage(MoveDirect direct);

	bool CheckDirect(BaseBlock::StageVector position, MoveDirect direct);

	void SetElement(BaseBlock::StageVector position, BaseBlock::Element element);
};