#pragma once

class GameScene;

class ISceneState {
public:

	virtual ~ISceneState() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void DrawUI() = 0;

};

