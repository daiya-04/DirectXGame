#pragma once
#include"Input.h"
class SceneManager;

class IScene{
public:

	virtual void Init() = 0;
	virtual void Update() = 0;
	
	virtual void DrawModel() = 0;
	virtual void DrawBackGround() = 0;
	virtual void DrawUI() = 0;
	virtual void DrawParticle() = 0;
	virtual void DrawParticleModel() = 0;

	virtual void DebugGUI() = 0;

	virtual ~IScene() {};

};

