#pragma once

#include <vector>
#include <memory>

#include "BaseBlock.h"

class BlockManager
{
private:

	typedef std::unique_ptr<BaseBlock> Block;

	std::vector<std::vector<std::vector<Block>>> data_;

public:

	void Initialize();

	void Update();

	void Draw();

};