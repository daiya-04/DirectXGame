#pragma once

#include "../BaseBlock.h"

class PlayerBlock : public BaseBlock
{
private:

	Object3d* exModel_ = nullptr;

public:

	void Initialize() override;

	//void Update() override;

	void Draw() override;

	void ApplyVariables(const char* groupName);
	void StorageVariables(const char* groupName);

	void AddModel(Object3d* model)override { exModel_ = model; }

private:

	void StagingInitialize() override;

	void StagingRoot() override;
	void StagingMove() override;
	void StagingStop() override;
	void StagingLoad() override;
	void StagingFall() override;

private:


};