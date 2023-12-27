#pragma once
#include "AbstractSceneFactory.h"
#include <memory>

class SceneFactory : public AbstractSceneFactory{
public:
	
public:

	
	std::unique_ptr<IScene> CreateScene(AbstractSceneFactory::SceneName scaneName) override;

};

