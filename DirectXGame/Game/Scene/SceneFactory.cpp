#include "SceneFactory.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "DebugTestScene.h"

std::unique_ptr<IScene> SceneFactory::CreateScene(const std::string& sceneName){

    std::unique_ptr<IScene> newScene = nullptr;

    if (sceneName == "Game") {
        newScene = std::make_unique<GameScene>();
    }
    if (sceneName == "Title") {
        newScene = std::make_unique<TitleScene>();
    }
    if (sceneName == "Debug") {
        newScene = std::make_unique<DebugTestScene>();
    }

    return newScene;
}
