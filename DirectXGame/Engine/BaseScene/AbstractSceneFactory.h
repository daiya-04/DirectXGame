#pragma once
#include "IScene.h"
#include <string>
#include <memory>

class AbstractSceneFactory {
public:

	virtual ~AbstractSceneFactory() = default;

	virtual std::unique_ptr<IScene> CreateScene(const std::string& sceneName) = 0;

};
