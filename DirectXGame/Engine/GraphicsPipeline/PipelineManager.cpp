#include "PipelineManager.h"
#include "SpriteGraphicsPipeline.h"
#include "LineGraphicsPipeline.h"
#include "Object3dGraphicsPipeline.h"
#include "SkinningGraphicsPipeline.h"
#include "SkinningComputePipeline.h"
#include "SkyBoxGraphicsPipeline.h"
#include "GPUParticleGraphicsPipeline.h"
#include "ParticleInitComputePipeline.h"
#include "ParticleEmitComputePipeline.h"
#include "ParticleUpdateComputePipeline.h"
#include "ParticleGraphicsPipeline.h"

namespace DaiEngine {
	PipelineManager* PipelineManager::GetInstance() {
		static PipelineManager instance;

		return &instance;
	}

	void PipelineManager::Init() {
		//スプライト
		graphicsPipelines_["Sprite"] = std::make_unique<SpriteGraphicsPipeline>();
		graphicsPipelines_["Sprite"]->Init();
		//線
		graphicsPipelines_["Line"] = std::make_unique<LineGraphicsPipeline>();
		graphicsPipelines_["Line"]->Init();
		//3Dオブジェクト
		graphicsPipelines_["Object3d"] = std::make_unique<Object3dGraphicsPipeline>();
		graphicsPipelines_["Object3d"]->Init();
		//スキニングオブジェクト
		graphicsPipelines_["Skinning"] = std::make_unique<SkinningGraphicsPipeline>();
		graphicsPipelines_["Skinning"]->Init();
		//スカイボックス
		graphicsPipelines_["SkyBox"] = std::make_unique<SkyBoxGraphicsPipeline>();
		graphicsPipelines_["SkyBox"]->Init();
		//GPUパーティクル
		graphicsPipelines_["GPUParticle"] = std::make_unique<GPUParticleGraphicsPipeline>();
		graphicsPipelines_["GPUParticle"]->Init();
		//パーティクル
		graphicsPipelines_["Particle"] = std::make_unique<ParticleGraphicsPipeline>();
		graphicsPipelines_["Particle"]->Init();


		//スキニング
		computePipelines_["Skinning"] = std::make_unique<SkinningComputePipeline>();
		computePipelines_["Skinning"]->Init();
		//パーティクル初期化
		computePipelines_["ParticleInit"] = std::make_unique<ParticleInitComputePipeline>();
		computePipelines_["ParticleInit"]->Init();
		//パーティクル発生
		computePipelines_["ParticleEmit"] = std::make_unique<ParticleEmitComputePipeline>();
		computePipelines_["ParticleEmit"]->Init();
		//パーティクル更新
		computePipelines_["ParticleUpdate"] = std::make_unique<ParticleUpdateComputePipeline>();
		computePipelines_["ParticleUpdate"]->Init();

	}


	void PipelineManager::preDraw(const std::string& pipelineType, BlendMode blendMode) {
		graphicsPipelines_[pipelineType]->preDraw(blendMode);
	}

	void PipelineManager::preDispatch(const std::string& pipelineType) {
		computePipelines_[pipelineType]->preDispatch();
	}
}
