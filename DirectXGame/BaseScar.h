#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include "WorldTransform.h"
#include "Camera.h"

class BaseScar {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
protected:

	enum class RootParameter {
		kWorldTransform,
		kCamera,
		kTexture,
		kMaskTex,
		kScarData,

		kParamNum,
	};

	struct VertexData {
		Vector4 pos_;
		Vector2 uv_;
	};

	struct ScarData {
		Vector4 color_;
		float threshold_;
	};

protected:

	static ID3D12Device* device_;
	static ID3D12GraphicsCommandList* commandList_;

protected:

	ComPtr<ID3D12Resource> worldMatBuff_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	ComPtr<ID3D12Resource> vertexBuff_;
	ComPtr<ID3D12Resource> indexBuff_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	ComPtr<ID3D12Resource> scarDataBuff_;
	ScarData* scarData_ = nullptr;

	uint32_t textureHandle_ = 0;
	uint32_t maskTex_ = 0;

protected:

	//座標
	Vector3 position_{};
	//倍率
	Vector2 scale_ = { 1.5f,1.5f };
	//回転
	float rotate_{};

	float threshold_ = 0.0f;

	Vector4 color_ = {};

	bool isEffect_ = false;

	int32_t lifeTime_ = 60 * 4;
	int32_t lifeTimer_ = lifeTime_;

protected:

	//初期化
	virtual void Init(uint32_t textureHandle);
	//更新
	virtual void Update();
	virtual void EffectUpdate() {};
	//描画
	virtual void Draw(const Camera& camera);

	virtual void EffectStart(const Vector3& pos);
	virtual void HeightAdjustment(float height) { position_.y = height; }

	virtual bool IsEffect() const { return isEffect_; }

private:

	void TransferVertex();

	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

};

