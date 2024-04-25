#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
#include <memory>

class Skydome{
private:

	std::unique_ptr<Object3d> obj_;

public:
	//初期化
	void Init(std::shared_ptr<Model> modelHandle);
	//更新
	void Update();
	//描画
	void Draw(const Camera& camera);

};

