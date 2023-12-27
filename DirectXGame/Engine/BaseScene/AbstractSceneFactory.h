#pragma once
#include "IScene.h"
#include <string>
#include <memory>

class AbstractSceneFactory {
public:
	enum class SceneName {
		Title,
		Select,
		Game,
		Result,
		GameOver,

	};
public:

	virtual ~AbstractSceneFactory() = default;

	virtual std::unique_ptr<IScene> CreateScene(AbstractSceneFactory::SceneName scaneName) = 0;

};
