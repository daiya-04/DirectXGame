#include "SceneFactory.h"
#include "GameScene.h"
#include "TitleScene.h"

std::unique_ptr<IScene> SceneFactory::CreateScene(const std::string& sceneName){

    std::unique_ptr<IScene> newScene = nullptr;

    if (sceneName == "Game") {
        newScene = std::make_unique<GameScene>();
    }
    else if (sceneName =="Title"){ 
        newScene = std::make_unique<TitleScene>();
    }

    return newScene;
}
