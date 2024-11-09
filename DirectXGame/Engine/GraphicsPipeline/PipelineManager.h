#pragma once
#include <map>
#include <string>
#include <memory>
#include "BaseGraphicsPipeline.h"
#include "BaseComputePipeline.h"


class PipelineManager {
public:

	static PipelineManager* GetInstance();

	void Init();

	void preDraw(const std::string& pipelineType);

	void preDispatch(const std::string& pipelineType);

private:

	std::map<std::string, std::unique_ptr<BaseGraphicsPipeline>> graphicsPipelines_;
	std::map<std::string, std::unique_ptr<BaseComputePipeline>> computePipelines_;

private:

	PipelineManager() = default;
	~PipelineManager() = default;
	PipelineManager(const PipelineManager&) = delete;
	PipelineManager& operator=(const PipelineManager*) = delete;

};

