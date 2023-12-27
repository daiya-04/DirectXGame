#include "SceneFactory.h"
#include "GameScene.h"
#include "TitleScene.h"

std::unique_ptr<IScene> SceneFactory::CreateScene(AbstractSceneFactory::SceneName sceneName){

    std::unique_ptr<IScene> newScene = nullptr;


    switch (sceneName){
    case SceneName::Title:
        newScene = std::make_unique<TitleScene>();
        break;
    case SceneName::Select:
        break;
    case SceneName::Game:
        newScene = std::make_unique<GameScene>();
        break;
    case SceneName::Result:
        break;
    case SceneName::GameOver:
        break;
    default:
        break;
    }

    return newScene;
}
