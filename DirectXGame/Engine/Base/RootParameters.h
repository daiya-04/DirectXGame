#pragma once

namespace SpriteParam {
	enum class RootParameter {
		kMaterial,
		kWVPMat,
		kTexture,

		kParamNum,
	};
}

namespace Object3dParam {
	enum class RootParameter {
		kMaterial,
		kWorldTransform,
		kCamera,
		kTexture,
		kEnvironmentTex,
		kDirectionLight,
		kPointLight,
		kSpotLight,

		kParamNum,
	};
}

namespace SkinningObjParam {
	enum class RootParameter {
		kMaterial,
		kWorldTransform,
		kCamera,
		kTexture,
		kEnvironmentTex,
		kDirectionLight,
		kDeadEffect,

		kParamNum,
	};

	enum class ComputeRootParam {
		kPalette,
		kInputVertex,
		kInfluence,
		kOutputVertex,
		kSkinningInfo,

		kParamNum,
	};
}

namespace SkyBoxParam {
	enum class RootParameter {
		kWorldTransform,
		kCamera,
		kTexture,

		kParamNum,
	};
}

namespace GPUParticleParam {
	enum class RootParameter {
		kParticleGPU,
		kPerView,
		kTexture,

		kParamNum,
	};

	enum class InitRootParam {
		kParticles,
		kFreeListIndex,
		kFreeList,
		kMaxNum,

		kParamNum,
	};

	enum class EmitterRootParam {
		kParticles,
		kEmitter,
		kPerFrame,
		kFreeListIndex,
		kFreeList,
		kMaxNum,
		kOverLifeTime,

		kParamNum,
	};

	enum class UpdateRootParam {
		kParticles,
			kPerFrame,
			kFreeListIndex,
			kFreeList,
			kMaxNum,
			kOverLifeTime,

			kParamNum,
	};
}

namespace ParticleParam {
	enum class RootParameter {
		kMaterial,
		kParticleGPU,
		kCamera,
		kTexture,

		kParamNum,
	};
}