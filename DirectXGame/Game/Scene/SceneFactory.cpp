#include "SceneFactory.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "DebugTestScene.h"

std::unique_ptr<IScene> SceneFactory::CreateScene(const std::string& sceneName){
    //新しいシーン
    std::unique_ptr<IScene> newScene = nullptr;

    if (sceneName == "Game") {
        //ゲームシーン生成
        newScene = std::make_unique<GameScene>();
    }
    if (sceneName == "Title") {
        //タイトルシーン生成
        newScene = std::make_unique<TitleScene>();
    }
    if (sceneName == "Debug") {
        //デバッグシーン生成
        newScene = std::make_unique<DebugTestScene>();
    }

    return newScene;
}
