#include "SceneFactory.h"
#include "GameScene.h"

std::unique_ptr<IScene> SceneFactory::CreateScene(const std::string& sceneName){

    std::unique_ptr<IScene> newScene = nullptr;

    if (sceneName == "Game") {
        newScene = std::make_unique<GameScene>();
    }

    return newScene;
}
