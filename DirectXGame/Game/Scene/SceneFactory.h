#pragma once
#include "AbstractSceneFactory.h"
#include <memory>

class SceneFactory : public AbstractSceneFactory{
public:

	std::unique_ptr<IScene> CreateScene(const std::string& sceneName) override;

};

