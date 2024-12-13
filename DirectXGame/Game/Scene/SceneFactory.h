#pragma once
///---------------------------------------------------------------------------------------------
// 
// シーンファクトリー
// シーンの生成を行う
// 
///---------------------------------------------------------------------------------------------

#include "AbstractSceneFactory.h"
#include <memory>

//シーンファクトリークラス
class SceneFactory : public AbstractSceneFactory{
public:
	/// <summary>
	/// シーン生成
	/// </summary>
	/// <param name="sceneName">シーンの名前</param>
	/// <returns>生成した新しいシーン</returns>
	std::unique_ptr<IScene> CreateScene(const std::string& sceneName) override;

};

