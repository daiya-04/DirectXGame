#include "BaseBlock.h"

//std::vector<uint32_t> BaseBlock::sHandles_;
//
//void BaseBlock::StaticInitialize()
//{
//	sHandles_.clear();
//
//	uint32_t handle = 0;
//	handle = TextureManager::LoadUv("stageTex.png", "Resources/block/");
//	sHandles_.push_back(handle);
//	handle = TextureManager::LoadUv("barrierTex.png", "Resources/block/");
//	sHandles_.push_back(handle);
//}

void BaseBlock::Initialize()
{
	Initialize({ 0.0f,0.0f,0.0f }, BlockType::kBlockStage);
}

void BaseBlock::Initialize(const Vector3& position, BlockType type)
{
	mapPosition_ = position;
	type_ = type;
	transformBase_.scale_ = { 1.0f,1.0f,1.0f };
	transformBase_.rotation_ = { 0.0f,0.0f,0.0f };
	transformBase_.translation_ = { 0.0f,0.0f,0.0f };

	switch (type_)
	{
	case BaseBlock::BlockType::kBlockStage:
		models_.emplace_back(Object3d::Create("plane"));
		models_[0]->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		break;
	case BaseBlock::BlockType::kBlockBarrier:
		models_.emplace_back(Object3d::Create("plane"));
		models_[0]->SetColor({ 0.5f,0.5f,0.5f,1.0f });
		break;
	case BaseBlock::BlockType::kCountofBlockType:
	default:
		break;
	}
}

void BaseBlock::Update()
{
	UpdateMatrixs();
}

void BaseBlock::Draw()
{
	DrawAllModel();
}