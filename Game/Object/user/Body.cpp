#include "Body.h"

void BodyBlock::Initialize()
{
	BaseBlock::Initialize();
}

void BodyBlock::Update()
{
	modelTransform_.translation_ = { mapPosition_.x * kBlockSize,mapPosition_.y * -kBlockSize,mapPosition_.z * kBlockSize };
	//BaseBlock::Update();
	if (stagingRequest_) {
		staging_ = stagingRequest_.value();
		switch (staging_)
		{
		case BaseBlock::kSROOT:
			StagingInitialize();
			break;
		case BaseBlock::kSMOVE:
			StagingInitialize();
			break;
		case BaseBlock::kSSTOP:
			StagingInitialize();
			break;
		case BaseBlock::kSLOAD:
			StagingInitialize();
			break;
		case BaseBlock::kSCOUNT:
			StagingInitialize();
			staging_ = kSROOT;
		default:
			break;
		}
		stagingRequest_ = std::nullopt;
	}
	// 演出を更新
	switch (staging_)
	{
	case BaseBlock::kSROOT:
		StagingRoot();
		break;
	case BaseBlock::kSMOVE:
		StagingMove();
		break;
	case BaseBlock::kSSTOP:
		StagingStop();
		break;
	case BaseBlock::kSLOAD:
		StagingLoad();
		break;
	case BaseBlock::kSCOUNT:
		// 呼び出されてはいけない
		assert(false);
	default:
		break;
	}
	modelTransform_.UpdateMatrix();
}

void BodyBlock::Draw()
{
	model_->Draw(modelTransform_, *viewProjection_);
}

void BodyBlock::ApplyVariables(const char* groupName)
{
	BaseBlock::ApplyVariables(groupName);
	groupName;
}

void BodyBlock::StorageVariables(const char* groupName)
{
	BaseBlock::StorageVariables(groupName);
	groupName;
}

void BodyBlock::StagingInitialize()
{
	BaseBlock::StagingInitialize();
}

void BodyBlock::StagingRoot()
{
	stagingFrame_++;


	// ループさせる
	if (cStagingFrames_[kSROOT] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}

}

void BodyBlock::StagingMove()
{
	stagingFrame_++;


	// 体に当たっている場合は積込をしたい
	if (cStagingFrames_[kSMOVE] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}
}

void BodyBlock::StagingStop()
{
	stagingFrame_++;



	if (cStagingFrames_[kSSTOP] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}
}

void BodyBlock::StagingLoad()
{
	stagingFrame_++;



	if (cStagingFrames_[kSLOAD] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}
}