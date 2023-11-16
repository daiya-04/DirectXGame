#include "StageObject.h"

void BaseBlock::Initialize()
{

	StagingInitialize();
	staging_ = kSROOT;


	// 関数を取得
	pStaging[kSROOT] = &BaseBlock::StagingRoot;
	pStaging[kSMOVE] = &BaseBlock::StagingMove;
	pStaging[kSSTOP] = &BaseBlock::StagingStop;
	pStaging[kSLOAD] = &BaseBlock::StagingLoad;
}

void BaseBlock::Update()
{
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
	(this->*pStaging[staging_])();
}

void BaseBlock::ApplyVariables(const char* groupName)
{
	// 設定を反映する

	groupName;
}

void BaseBlock::StorageVariables(const char* groupName)
{
	// 設定を保存する

	groupName;
}

void BaseBlock::StagingInitialize()
{
	stagingFrame_ = 0;
}

void BaseBlock::StagingRoot()
{

	stagingFrame_++;



	// ループさせる
	if (cStagingFrames_[kSROOT] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}

}

void BaseBlock::StagingMove()
{
	stagingFrame_++;


	// 体に当たっている場合は積込をしたい
	if (cStagingFrames_[kSMOVE] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}
}

void BaseBlock::StagingStop()
{
	stagingFrame_++;



	if (cStagingFrames_[kSSTOP] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}
}

void BaseBlock::StagingLoad()
{
	stagingFrame_++;



	if (cStagingFrames_[kSLOAD] <= stagingFrame_) {
		stagingRequest_ = kSROOT;
	}
}
