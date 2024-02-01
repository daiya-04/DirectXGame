#include "SceneFactory.h"
#include "GameScene.h"
#include "TitleScene.h"
#include"SelectScene.h"
#include"ResultScene.h"
#include"TutorialScene.h"

std::unique_ptr<IScene> SceneFactory::CreateScene(AbstractSceneFactory::SceneName sceneName){

    std::unique_ptr<IScene> newScene = nullptr;


    switch (sceneName){
    case SceneName::Title:
        newScene = std::make_unique<TitleScene>();
        break;
    case SceneName::Select:
        newScene = std::make_unique<SelectScene>();
        break;
    case SceneName::Game:
        newScene = std::make_unique<GameScene>();
        break;
    case SceneName::Result:
        newScene = std::make_unique<ResultScene>();
        break;
    case SceneName::GameOver:
        break;
    case SceneName::Tutorial:
        newScene = std::make_unique<TutorialScene>();
        break;
    default:
        break;
    }

    return newScene;
}
