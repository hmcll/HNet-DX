#pragma once

#include "Include.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "tiny_obj_loader.h"

class Window {
	HWND hWnd;
	HINSTANCE hInstance;
	static std::unique_ptr<Window> _this;
	Window() = default;


	WNDCLASSEX ConstructWindowClass();

	DXGI_SWAP_CHAIN_DESC ConstructSwapChainDescription();

	void CreateDeviceAndSwapChain();

	void InitalizeImGui();

public:
	friend std::unique_ptr<Window> std::make_unique<Window>();
	static void Initalize(HINSTANCE hInstance, LPCWSTR WindowName);


	::Microsoft::WRL::ComPtr < IDXGISwapChain> swapchain;
	::Microsoft::WRL::ComPtr < ID3D11Device> device;
	::Microsoft::WRL::ComPtr < ID3D11DeviceContext> context;

	::Microsoft::WRL::ComPtr < ID3D11Resource> BackBuffer_resource;
	::Microsoft::WRL::ComPtr < ID3D11RenderTargetView> BackBuffer_Target;

	D3D11_VIEWPORT viewport;

	static Window& Get();
	WPARAM MessageLoop(std::function<void()>func);


};

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
