#pragma once

#include <vector>

#include "BaseBlock.h"
#include "../../Input.h"

class BlockManager;

/// <summary>
/// シングルトンパターンにしたい
/// </summary>
class MapManager final
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
	BlockManager* blockManager_ = nullptr;

	// 演出中でないことを確認する
	bool isStaging_ = false;

public:

	static MapManager* GetInstance();

	void Initialize();

	void Update();

	void Draw();

	void DebugGUI();

	// ステージ情報を設定する関数
	void SetStageData(const StageArray<BaseBlock::Element>& data);

private:

	MapManager() = default;
	~MapManager() = default;
	MapManager(const MapManager& obj) = delete;
	const MapManager& operator=(const MapManager& obj) = delete;

	// 走査によるフラグの管理のみをしたい
	// 今は変更までしてしまっている
	void GetOperate();

	// 重くなりそうなので最初の一回のみ
	void GetPlayerPosition();

	// 動かす為の処理をまとめる
	void MoveMainObject(MoveDirect direct);

	// 方向を確認する
	// プレイヤーの移動のみ対応
	bool CheckDirectPlayer(BaseBlock::StageVector position, MoveDirect direct, BaseBlock::Element element);
	bool CheckDirectBody(BaseBlock::StageVector position, MoveDirect direct, BaseBlock::Element element,BaseBlock::StageVector limit);

	// 場所を指定して属性変更
	void SetElement(BaseBlock::StageVector position, BaseBlock::Element element);
	const BaseBlock::Element& GetElement(BaseBlock::StageVector position);

	// 移動先に体がないことを確認する
	// 再起関数?
	BaseBlock::StageVector GetOnBody(const BaseBlock::StageVector& pos);

};