#pragma once

#include "../BaseBlock.h"

class HeadBlock : public BaseBlock
{
private:



public:

	void Initialize() override;

	//void Update() override;

	void Draw() override;

	void ApplyVariables(const char* groupName);
	void StorageVariables(const char* groupName);

private:

	void StagingInitialize() override;

	void StagingRoot() override;
	void StagingLoad() override;

private:


};