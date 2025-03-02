#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include <vector>
#include "WorldTransform.h"
#include "Camera.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "ModelManager.h"
#include "Animation.h"
#include "PipelineManager.h"

namespace DaiEngine {
	class Object3d {
	private:
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public: //静的メンバ関数
		//モデルの生成
		static Object3d* Create(std::shared_ptr<Model> model);
		//描画前処理
		static void preDraw();
		//描画後処理
		static void postDraw();

		static void SetPointLight(PointLight* pointLight) { pointLight_ = pointLight; }

		static void SetSpotLight(SpotLight* spotLight) { spotLight_ = spotLight; }

	public:

		static PointLight* pointLight_;
		static SpotLight* spotLight_;

	private: //メンバ変数

		std::shared_ptr<Model> model_;

		bool visible_ = true;

	public:

		WorldTransform worldTransform_;

	public: //メンバ関数

		//初期化
		void Initialize(std::shared_ptr<Model> model);
		//更新
		void Update();
		//描画
		void Draw(const Camera& camera, BlendMode blendMode = BlendMode::kAlpha);

		void SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }

		void SetModelHandle(std::shared_ptr<Model> model) { model_ = model; }

		Vector3 GetWorldPos() const;
		std::shared_ptr<Model> GetModel() const { return model_; }

		void SetVisible(bool visible) { visible_ = visible; }

	};
}
