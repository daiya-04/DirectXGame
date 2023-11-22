#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include "WinApp.h"
#include <thread>
#include <chrono>
#include <string>

class DirectXCommon{
private:

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	uint32_t kSrvHeapDescriptorNum = 512;

	WinApp* win_ = nullptr;

	ComPtr<ID3D12Device> device_;
	ComPtr<IDXGIFactory7> dxgiFactory_;
	ComPtr<ID3D12CommandQueue> commandQueue_;
	ComPtr<ID3D12CommandAllocator> commandAllocator_;
	ComPtr<ID3D12GraphicsCommandList> commandList_;
	ComPtr<IDXGISwapChain4> swapChain_;
	std::vector<ComPtr<ID3D12Resource>> backBuffers_;
	ComPtr<ID3D12Resource> depthBuffer_;
	ComPtr<ID3D12Fence> fence_;
	uint64_t fenceValue = 0;

	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;

	int32_t srvHeapCount_ = 0;

public:

	static DirectXCommon* GetInstance();

	//初期化
	void Initialize(WinApp* win);
	//デバイスの初期化
	void InitializeDevice();
	//コマンド関連の初期化
	void InitializeCommand();
	//スワップチェインの初期化
	void InitializeSwapChain();
	//レンダーターゲットビューの初期化
	void InitializeRenderTargetView();
	//深度バッファの初期化
	void InitializeDepthBuffer();
	//フェンスの初期化
	void InitializeFence();
	//描画前処理
	void preDraw();
	//描画後処理
	void postDraw();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	ID3D12Device* GetDevice() const { return device_.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
	size_t GetBackBufferCount() const { return backBuffers_.size(); }
	ID3D12DescriptorHeap* GetSrvHeap() { return srvDescriptorHeap_.Get(); }
	int32_t GetSrvHeapCount() { return srvHeapCount_; }
	void IncrementSrvHeapCount() { srvHeapCount_++; }

private:

	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;

private:
	DirectXCommon() = default;
	~DirectXCommon() = default;
	DirectXCommon(const DirectXCommon&) = delete;
	DirectXCommon& operator=(const DirectXCommon&) = delete;

	//FPS固定初期化
	void InitializeFixFPS();
	//FPS固定更新
	void UpdateFixFPS();

	//ログ
	void Log(const std::string& message);

	std::wstring ConvertString(const std::string& str);

	std::string ConvertString(const std::wstring& str);

};

