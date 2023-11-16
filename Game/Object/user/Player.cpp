#include "Player.h"

void PlayerBlock::Initialize()
{
	BaseBlock::Initialize();

	// 関数を取得
	pStaging[kSROOT] = &PlayerBlock::StagingRoot;
	pStaging[kSMOVE] = &PlayerBlock::StagingMove;
	pStaging[kSSTOP] = &PlayerBlock::StagingStop;
	pStaging[kSLOAD] = &PlayerBlock::StagingLoad;
}

void PlayerBlock::Update()
{
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
