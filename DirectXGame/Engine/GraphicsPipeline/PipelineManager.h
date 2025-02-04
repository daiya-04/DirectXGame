#pragma once
///---------------------------------------------------------------------------------------------
//
// PipelineManager
// パイプライン管理
//
///---------------------------------------------------------------------------------------------


#include <map>
#include <string>
#include <memory>
#include "BaseGraphicsPipeline.h"
#include "BaseComputePipeline.h"

namespace DaiEngine {
	class PipelineManager {
	public:
		/// <summary>
		/// インスタンス取得
		/// </summary>
		/// <returns></returns>
		static PipelineManager* GetInstance();
		/// <summary>
		/// 初期化
		/// </summary>
		void Init();
		/// <summary>
		/// 描画前
		/// </summary>
		/// <param name="pipelineType"></param>
		void preDraw(const std::string& pipelineType);
		/// <summary>
		/// ディスパッチ起動前
		/// </summary>
		/// <param name="pipelineType"></param>
		void preDispatch(const std::string& pipelineType);
		/// <summary>
		/// スクリーンブレンド描画前(苦し紛れ)
		/// </summary>
		void preDrawScreen();

	private:
		//グラフィックスパイプラインまとめ
		std::map<std::string, std::unique_ptr<BaseGraphicsPipeline>> graphicsPipelines_;
		//コンピュートパイプラインまとめ
		std::map<std::string, std::unique_ptr<BaseComputePipeline>> computePipelines_;

	private:

		PipelineManager() = default;
		~PipelineManager() = default;
		PipelineManager(const PipelineManager&) = delete;
		PipelineManager& operator=(const PipelineManager*) = delete;

	};
}
