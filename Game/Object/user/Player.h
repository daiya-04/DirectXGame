#pragma once

#include "../StageObject.h"

class PlayerBlock : public BaseBlock
{
public:

	void Initialize() override;

	void Update() override;

	void Draw() override;

	void ApplyVariables(const char* groupName);
	void StorageVariables(const char* groupName);
};