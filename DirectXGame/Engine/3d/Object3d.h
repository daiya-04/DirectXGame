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

	private:

		struct ObjectData {
			//透過光の色
			Vector3 subsurfaceColor_;
			//光が透ける強さ（影部分の光の強さ）
			float subsurfaceIntensity_;
			//フレネル反射の色
			Vector3 fresnelColor_;
			//フレネル反射の強さ
			float fresnelPower_;
			//フレネル反射の影響度
			float fresnelIntensity_;
			//環境マップの影響度
			float envReflectIntensity_;

			float alpha_;
		};

	public: //静的メンバ関数
		//モデルの生成
		static Object3d* Create(std::shared_ptr<Model> model);
		//描画前処理
		static void preDraw();
		//描画後処理
		static void postDraw();

		static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

		static void SetPointLight(PointLight* pointLight) { pointLight_ = pointLight; }

		static void SetSpotLight(SpotLight* spotLight) { spotLight_ = spotLight; }

	public:

		static PointLight* pointLight_;
		static SpotLight* spotLight_;

	private: //メンバ変数

		std::shared_ptr<Model> model_;

		ComPtr<ID3D12Resource> objectDataBuff_;
		ObjectData* objectData_ = nullptr;

		bool visible_ = true;



	public:

		WorldTransform worldTransform_;

	public: //メンバ関数

		//初期化
		void Initialize(std::shared_ptr<Model> model);
		//描画
		void Draw(const Camera& camera, BlendMode blendMode = BlendMode::kAlpha);

		void SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }

		void SetModelHandle(std::shared_ptr<Model> model) { model_ = model; }

		Vector3 GetWorldPos() const;
		std::shared_ptr<Model> GetModel() const { return model_; }

		void SetVisible(bool visible) { visible_ = visible; }
		void SetAplha(float alpha) { objectData_->alpha_ = alpha; }
		void SetSubsurfaceColor(const Vector3& color) { objectData_->subsurfaceColor_ = color; }
		void SetSubsurfaceIntensity(float intensity) { objectData_->subsurfaceIntensity_ = std::clamp(intensity, 0.0f, 1.0f); }
		void SetFresnelColor(const Vector3& color) { objectData_->fresnelColor_ = color; }
		void SetFresnelPower(float power) { objectData_->fresnelPower_ = power; }
		void SetFresnelIntensity(float intensity) { objectData_->fresnelIntensity_ = std::clamp(intensity, 0.0f, 1.0f); }
		void SetEnvReflectIntensity(float intensity) { objectData_->envReflectIntensity_ = std::clamp(intensity, 0.0f, 1.0f); }


	};
}
