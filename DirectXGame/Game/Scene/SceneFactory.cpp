#include "SceneFactory.h"
#include "GameScene.h"
#include "ModelScene.h"
#include "SpriteScene.h"

std::unique_ptr<IScene> SceneFactory::CreateScene(const std::string& sceneName){

    std::unique_ptr<IScene> newScene = nullptr;

    if (sceneName == "Game") {
        newScene = std::make_unique<GameScene>();
    }else if (sceneName == "Model") {
        newScene = std::make_unique<ModelScene>();
    }else if (sceneName == "Sprite") {
        newScene = std::make_unique<SpriteScene>();
    }

    return newScene;
}
