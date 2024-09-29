#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "LevelLoader.h"

#include <memory>

class Rock {
public:

	void Init(const std::shared_ptr<Model>& model);

	void Update();

	void Draw(const Camera& camera);

	void SetData(const LevelData::ObjectData& data);

private:

	std::unique_ptr<Object3d> obj_;

};

