#pragma once

#include <vector>
#include <memory>
#include <array>
#include <list>

#include "BaseBlock.h"
#include "MapManager.h"
#include "../../ModelManager.h"

/// <summary>
/// シングルトンパターンにしたい
/// モデルの切り替えをどうするか悩み中
/// </summary>
class BlockManager final
{
private:

	typedef std::unique_ptr<BaseBlock> pBlock;

	//std::vector<std::vector<std::vector<pBlock>>> currentData_;

	// おおよそ 65 らしい
	static const size_t kMaxNormalBlockNum_ = 70;
	
	// 動かす頭
	static const size_t kMaxPlayerBlockNum_ = 1;

	// 分け与えなければいけない頭
	static const size_t kMaxHeadBlockNum_ = 10;
	
	// 配置される予定の体
	static const size_t kMaxBodyBlockNum_ = 10;

	std::list<pBlock> listBlock_;

	typedef std::unique_ptr<Object3d> pModel;

	std::array<pModel, kMaxNormalBlockNum_> arrModelNormal_;
	std::array<pModel, kMaxPlayerBlockNum_> arrModelPlayer_;
	std::array<pModel, kMaxHeadBlockNum_> arrModelHead_;
	std::array<pModel, kMaxBodyBlockNum_> arrModelBody_;

	uint32_t iNormalModel_ = 0;
	uint32_t iPlayerModel_ = 0;
	uint32_t iBodyModel_ = 0;
	uint32_t iHeadModel_ = 0;

	ViewProjection* vp_ = nullptr;

	// マップのサイズ
	size_t kMapSize = 0;

	// 
	MapManager::StageArray<BaseBlock*> mapBlock_;

public:

	static BlockManager* GetInstance();

	void Initialize();

	void Reset();

	void Update();

	void Draw();

	// ステージ情報を設定する関数
	void SetStageData(const MapManager::StageArray<BaseBlock::Element> data);

	void SetViewProjection(ViewProjection* view) { vp_ = view; }

	// マップの場所を保存して指定できるようにする
	void SetBlockPosition(const BaseBlock::StageVector& prePos,const BaseBlock::StageVector& pos);

	bool GetIsStaging();

private:

	BlockManager() = default;
	~BlockManager() = default;
	BlockManager(const BlockManager& obj) = delete;
	const BlockManager& operator=(const BlockManager& obj) = delete;

	BaseBlock* CreateNormalBlock(const BaseBlock::StageVector& pos);
	BaseBlock* CreatePlayerBlock(const BaseBlock::StageVector& pos);
	BaseBlock* CreateHeadBlock(const BaseBlock::StageVector& pos);
	BaseBlock* CreateBodyBlock(const BaseBlock::StageVector& pos);

};