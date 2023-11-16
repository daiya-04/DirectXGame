#include "Player.h"

void PlayerBlock::Initialize()
{
	BaseBlock::Initialize();
	;
}

void PlayerBlock::Update()
{
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
}

void PlayerBlock::Draw()
{
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
}

void PlayerBlock::StagingRoot()
{
}

void PlayerBlock::StagingMove()
{
}

void PlayerBlock::StagingStop()
{
}

void PlayerBlock::StagingLoad()
{
}
