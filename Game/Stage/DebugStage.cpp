#include "DebugStage.h"

DebugStage::DebugStage()
{
}

DebugStage::~DebugStage()
{
}

void DebugStage::Initialize()
{
	input_ = Input::GetInstance();

	LoadStageData();
	ApplyStageData();
}

void DebugStage::Reset()
{
	//LoadStageData();
	currentData_ = kStageData_;
	// ステージの情報を反映
	ApplyStageData();
}

void DebugStage::Update()
{
}

void DebugStage::Draw()
{
}

void DebugStage::LoadStageData()
{
}

void DebugStage::SaveStageData()
{
}

void DebugStage::ApplyStageData()
{
}
