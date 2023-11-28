#include "Head.h"

#include "../../../ImGuiManager.h"

void HeadBlock::Initialize()
{
	BaseBlock::Initialize();
	element_ = Element::kHead;
}
void HeadBlock::Draw()
{
	model_->Draw(modelTransform_, *viewProjection_);
}

void HeadBlock::ApplyVariables(const char* groupName)
{
	BaseBlock::ApplyVariables(groupName);
	groupName;
}

void HeadBlock::StorageVariables(const char* groupName)
{
	BaseBlock::StorageVariables(groupName);
	groupName;
}

void HeadBlock::StagingInitialize()
{
	BaseBlock::StagingInitialize();
}

void HeadBlock::StagingRoot()
{
	stagingFrame_++;


	// ループさせる
	if (cStagingFrames_[kSROOT] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}

}
void HeadBlock::StagingLoad()
{
	stagingFrame_++;



	if (cStagingFrames_[kSLOAD] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}
}