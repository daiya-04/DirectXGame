#include "Block.h"

#include "../../../ImGuiManager.h"

void NormalBlock::Initialize()
{
	BaseBlock::Initialize();
	element_ = Element::kBlock;
}
void NormalBlock::Draw()
{
	model_->Draw(modelTransform_, *viewProjection_);
}

void NormalBlock::ApplyVariables(const char* groupName)
{
	BaseBlock::ApplyVariables(groupName);
	groupName;
}

void NormalBlock::StorageVariables(const char* groupName)
{
	BaseBlock::StorageVariables(groupName);
	groupName;
}

void NormalBlock::StagingInitialize()
{
	BaseBlock::StagingInitialize();
}

void NormalBlock::StagingRoot()
{
	stagingFrame_++;


	// ループさせる
	if (cStagingFrames_[kSROOT] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}

}

void NormalBlock::StagingMove()
{
	stagingFrame_++;


	// 体に当たっている場合は積込をしたい
	if (cStagingFrames_[kSMOVE] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}
}

void NormalBlock::StagingStop()
{
	stagingFrame_++;



	if (cStagingFrames_[kSSTOP] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}
}

void NormalBlock::StagingLoad()
{
	stagingFrame_++;



	if (cStagingFrames_[kSLOAD] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}
}