#include "BlockManager.h"

#include "user/Player.h"
#include "user/Block.h"

void BlockManager::Initialize()
{
	BaseBlock::StageVector position{ 0,0,0 };
	currentData_.reserve(5);
	// x 軸
	for (size_t i = 0; i < currentData_.size(); i++)
	{
		position.x = i;
		currentData_[i].reserve(5);
		// y 軸
		for (size_t j = 0; j < currentData_[i].size(); j++)
		{
			position.y = j;
			currentData_[i][j].reserve(5);
			// z 軸
			for (size_t k = 0; k < currentData_[i][j].size(); k++)
			{
				position.z = k;
				BaseBlock* block = new NormalBlock;
				block->Initialize(position);
				//block->SetViewProjection()
				currentData_[i][j][k].reset(block);
			}
		}
	}
}

void BlockManager::Update()
{
	// x 軸
	for (size_t i = 0; i < currentData_.size(); i++)
	{
		// y 軸
		for (size_t j = 0; j < currentData_[i].size(); j++)
		{
			// z 軸
			for (size_t k = 0; k < currentData_[i][j].size(); k++)
			{
				pBlock& block = currentData_[i][j][k];
				block->Update();
			}
		}
	}
}

void BlockManager::Draw()
{
	// x 軸
	for (size_t i = 0; i < currentData_.size(); i++)
	{
		// y 軸
		for (size_t j = 0; j < currentData_[i].size(); j++)
		{
			// z 軸
			for (size_t k = 0; k < currentData_[i][j].size(); k++)
			{
				pBlock& block = currentData_[i][j][k];
				block->Draw();
			}
		}
	}
}

void BlockManager::SetModel(BaseBlock::Element elment)
{
	// x 軸
	for (size_t i = 0; i < currentData_.size(); i++)
	{
		// y 軸
		for (size_t j = 0; j < currentData_[i].size(); j++)
		{
			// z 軸
			for (size_t k = 0; k < currentData_[i][j].size(); k++)
			{
				pBlock& block = currentData_[i][j][k];
				block->Draw();
			}
		}
	}
}
