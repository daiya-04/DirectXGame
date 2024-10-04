#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
#include "LevelLoader.h"
#include <memory>

class Ground{
private:

	std::unique_ptr<Object3d> obj_;

public:

	void Init(std::shared_ptr<Model> modelHandle);

	void Update();

	void Draw(const Camera& camera);

	void SetData(const LevelData::ObjectData& data);

};

