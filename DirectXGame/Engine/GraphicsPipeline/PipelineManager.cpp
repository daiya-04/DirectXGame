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


PipelineManager* PipelineManager::GetInstance() {
	static PipelineManager instance;

	return &instance;
}

void PipelineManager::Init() {

	graphicsPipelines_["Sprite"] = std::make_unique<SpriteGraphicsPipeline>();
	graphicsPipelines_["Sprite"]->Init();

	graphicsPipelines_["Line"] = std::make_unique<LineGraphicsPipeline>();
	graphicsPipelines_["Line"]->Init();

	graphicsPipelines_["Object3d"] = std::make_unique<Object3dGraphicsPipeline>();
	graphicsPipelines_["Object3d"]->Init();

	graphicsPipelines_["Skinning"] = std::make_unique<SkinningGraphicsPipeline>();
	graphicsPipelines_["Skinning"]->Init();

	graphicsPipelines_["SkyBox"] = std::make_unique<SkyBoxGraphicsPipeline>();
	graphicsPipelines_["SkyBox"]->Init();

	graphicsPipelines_["GPUParticle"] = std::make_unique<GPUParticleGraphicsPipeline>();
	graphicsPipelines_["GPUParticle"]->Init();

	graphicsPipelines_["Particle"] = std::make_unique<ParticleGraphicsPipeline>();
	graphicsPipelines_["Particle"]->Init();



	computePipelines_["Skinning"] = std::make_unique<SkinningComputePipeline>();
	computePipelines_["Skinning"]->Init();

	computePipelines_["ParticleInit"] = std::make_unique<ParticleInitComputePipeline>();
	computePipelines_["ParticleInit"]->Init();

	computePipelines_["ParticleEmit"] = std::make_unique<ParticleEmitComputePipeline>();
	computePipelines_["ParticleEmit"]->Init();

	computePipelines_["ParticleUpdate"] = std::make_unique<ParticleUpdateComputePipeline>();
	computePipelines_["ParticleUpdate"]->Init();

}


void PipelineManager::preDraw(const std::string& pipelineType) {
	graphicsPipelines_[pipelineType]->preDraw();
}

void PipelineManager::preDispatch(const std::string& pipelineType) {
	computePipelines_[pipelineType]->preDispatch();
}
