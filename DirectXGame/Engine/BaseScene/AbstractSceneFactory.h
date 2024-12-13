#pragma once
#include "IScene.h"
#include <string>
#include <memory>

//シーン生成クラスの抽象クラス
class AbstractSceneFactory {
public:

	virtual ~AbstractSceneFactory() = default;
	//シーン生成
	virtual std::unique_ptr<IScene> CreateScene(const std::string& sceneName) = 0;

};
