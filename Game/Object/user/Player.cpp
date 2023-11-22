#include "Player.h"

void PlayerBlock::Initialize()
{
	BaseBlock::Initialize();
}

void PlayerBlock::Update()
{
	modelPosition_.translation_ = { mapPosition_.x * kBlockSize,mapPosition_.y * -kBlockSize,mapPosition_.z * kBlockSize };
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
	modelPosition_.UpdateMatrix();
}

void PlayerBlock::Draw()
{
	model_->Draw(modelPosition_, *viewProjection_);
}

void PlayerBlock::ApplyVariables(const char* groupName)
{
	BaseBlock::ApplyVariables(groupName);
	groupName;
}

void PlayerBlock::StorageVariables(const char* groupName)
{
	BaseBlock::StorageVariables(groupName);
	groupName;
}

void PlayerBlock::StagingInitialize()
{
	BaseBlock::StagingInitialize();
}

void PlayerBlock::StagingRoot()
{
	stagingFrame_++;

	// 操作を受け付ける


	// ループさせる
	if (cStagingFrames_[kSROOT] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}

}

void PlayerBlock::StagingMove()
{
	stagingFrame_++;


	// 体に当たっている場合は積込をしたい
	if (cStagingFrames_[kSMOVE] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}
}

void PlayerBlock::StagingStop()
{
	stagingFrame_++;



	if (cStagingFrames_[kSSTOP] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}
}

void PlayerBlock::StagingLoad()
{
	stagingFrame_++;



	if (cStagingFrames_[kSLOAD] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}
}