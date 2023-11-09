#include "DebugStage.h"

DebugStage::DebugStage()
{
}

DebugStage::~DebugStage()
{
}

void DebugStage::Initialize()
{
	LoadStageData();
}

void DebugStage::Reset()
{
	currentData_ = kStageData_;
	// ステージの情報を反映
}

void DebugStage::Update()
{
}

void DebugStage::Draw()
{
}

void DebugStage::LoadStageData()
{
	for (size_t i = 0; i < 5; i++)
	{
		for (size_t j = 0; j < 5; j++)
		{
			kStageData_.blockData_.push_back({ (float)j - 3,(float)i - 3, 0 });
		}
	}
	kStageData_.playerData_ = { 0,0,1 };
}

void DebugStage::SaveStageData()
{

}
