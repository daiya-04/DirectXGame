#include "BlockManager.h"

#include "BlockList.h"
#include "../../ImGuiManager.h"

using Element = BaseBlock::Element;

BlockManager* BlockManager::GetInstance()
{
	static BlockManager instance;
	return &instance;
}

void BlockManager::Initialize()
{
	uint32_t modelHandle = ModelManager::Load("Box");
	for (size_t i = 0; i < kMaxNormalBlockNum_; i++)
	{
		arrModelNormal_[i].reset(Object3d::Create(modelHandle));
	}
	modelHandle = ModelManager::Load("Box");
	for (size_t i = 0; i < kMaxPlayerBlockNum_; i++)
	{
		arrModelPlayer_[i].reset(Object3d::Create(modelHandle));
	}
	modelHandle = ModelManager::Load("Box");
	for (size_t i = 0; i < kMaxHeadBlockNum_; i++)
	{
		arrModelHead_[i].reset(Object3d::Create(modelHandle));
	}
	modelHandle = ModelManager::Load("Box");
	for (size_t i = 0; i < kMaxBodyBlockNum_; i++)
	{
		arrModelBody_[i].reset(Object3d::Create(modelHandle));
	}
	Reset();
}

void BlockManager::Reset()
{
	listBlock_.clear();
	iNormalModel_ = 0;
	iPlayerModel_ = 0;
	iBodyModel_ = 0;
	iHeadModel_ = 0;
	mapBlock_.clear();
	mapBlock_.resize(kMapSize);
	for (size_t i = 0; i < kMapSize; i++)
	{
		mapBlock_[i].resize(kMapSize);
		for (size_t j = 0; j < kMapSize; j++)
		{
			mapBlock_[i][j].resize(kMapSize);
			for (size_t k = 0; k < kMapSize; k++)
			{
				mapBlock_[i][j][k] = nullptr;
			}
		}
	}
}

void BlockManager::Update()
{
	std::list<pBlock>::iterator itr = listBlock_.begin();
	for (; itr != listBlock_.end(); ++itr)
	{
		itr->get()->Update();
	}
}

void BlockManager::Draw()
{
	std::list<pBlock>::iterator itr = listBlock_.begin();
	for (; itr != listBlock_.end(); ++itr)
	{
		itr->get()->Draw();
	}
}

void BlockManager::SetStageData(const MapManager::StageArray<BaseBlock::Element> data)
{
	kMapSize = data.size();
	Reset();
	BaseBlock* block = nullptr;
	for (size_t i = 0; i < data.size(); i++)
	{
		for (size_t j = 0; j < data[i].size(); j++)
		{
			for (size_t k = 0; k < data[i][j].size(); k++)
			{
				switch (data[i][j][k])
				{
				case  Element::kNone:
					break;
				case  Element::kPlayer:
					block = new PlayerBlock;
					listBlock_.emplace_back(CreatePlayerBlock({ i,j,k }));
					break;
				case  Element::kBody:
					listBlock_.emplace_back(CreateBodyBlock({ i,j,k }));
					break;
				case  Element::kBlock:
					listBlock_.emplace_back(CreateNormalBlock({ i,j,k }));
					break;
				default:
					break;
				}
			}
		}
	}
}

void BlockManager::SetBlockPosition(const BaseBlock::StageVector& prePos, const BaseBlock::StageVector& pos)
{
	mapBlock_[pos.x][pos.y][pos.z] = mapBlock_[prePos.x][prePos.y][prePos.z];
	mapBlock_[prePos.x][prePos.y][prePos.z] = nullptr;

	mapBlock_[pos.x][pos.y][pos.z]->SetMapPosition(pos);
}


BaseBlock* BlockManager::CreateNormalBlock(const BaseBlock::StageVector& pos) {
	BaseBlock* block = new NormalBlock;
	block->Initialize(pos);
	block->SetViewProjection(vp_);
	block->SetModel(arrModelNormal_[iNormalModel_].get());
	iNormalModel_++;
	mapBlock_[pos.x][pos.y][pos.z] = block;
	return block;
}
BaseBlock* BlockManager::CreatePlayerBlock(const BaseBlock::StageVector& pos) {

	BaseBlock* block = new PlayerBlock;
	block->Initialize(pos);
	block->SetViewProjection(vp_);
	block->SetModel(arrModelPlayer_[iPlayerModel_].get());
	iNormalModel_++;
	mapBlock_[pos.x][pos.y][pos.z] = block;
	return block;
}

BaseBlock* BlockManager::CreateHeadBlock(const BaseBlock::StageVector& pos) {

	BaseBlock* block = new NormalBlock;
	block->Initialize(pos);
	block->SetViewProjection(vp_);
	block->SetModel(arrModelHead_[iHeadModel_].get());
	iHeadModel_++;
	mapBlock_[pos.x][pos.y][pos.z] = block;
	return block;
}
BaseBlock* BlockManager::CreateBodyBlock(const BaseBlock::StageVector& pos) {

	BaseBlock* block = new BodyBlock;
	block->Initialize(pos);
	block->SetViewProjection(vp_);
	block->SetModel(arrModelBody_[iBodyModel_].get());
	iBodyModel_++;
	mapBlock_[pos.x][pos.y][pos.z] = block;
	return block;
}