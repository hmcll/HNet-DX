#include "Window.h"
#include "Include.h"

std::unique_ptr<Window> Window::_this = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

inline WNDCLASSEX Window::ConstructWindowClass() {
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = L"MainWindowClass";
	wc.hIconSm = nullptr;
	return wc;
}

DXGI_SWAP_CHAIN_DESC Window::ConstructSwapChainDescription() {
	DXGI_SWAP_CHAIN_DESC swapchaindesc;
	swapchaindesc.BufferDesc.Width = 0;
	swapchaindesc.BufferDesc.Height = 0;
	swapchaindesc.BufferDesc.RefreshRate.Numerator = 0;
	swapchaindesc.BufferDesc.RefreshRate.Denominator = 0;
	swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapchaindesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapchaindesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapchaindesc.SampleDesc.Count = 1;
	swapchaindesc.SampleDesc.Quality = 0;
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchaindesc.BufferCount = 1;
	swapchaindesc.OutputWindow = hWnd;
	swapchaindesc.Windowed = true;
	swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapchaindesc.Flags = 0;
	return swapchaindesc;
}

void Window::CreateDeviceAndSwapChain() {
	DXGI_SWAP_CHAIN_DESC swapchaindesc = ConstructSwapChainDescription();
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, &swapchaindesc, &swapchain, &device, nullptr, &context);

	swapchain->GetBuffer(0, IID_PPV_ARGS(&BackBuffer_resource));
	device->CreateRenderTargetView(BackBuffer_resource.Get(), nullptr, &BackBuffer_Target);
}

void Window::InitalizeImGui() {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device.Get(), context.Get());

}

void Window::Initalize(HINSTANCE hInstance, LPCWSTR WindowName) {
	_this = std::make_unique<Window>();
	_this->hInstance = hInstance;
	WNDCLASSEX wc = _this->ConstructWindowClass();

	_this->viewport.Width = 1920;
	_this->viewport.Height = 1080;
	_this->viewport.MinDepth = 0;
	_this->viewport.MaxDepth = 1;
	_this->viewport.TopLeftX = 0;
	_this->viewport.TopLeftY = 0;
	RegisterClassEx(&wc);

	_this->hWnd = CreateWindowEx(0, wc.lpszClassName, WindowName, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 10, 10, _this->viewport.Width, _this->viewport.Height, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(_this->hWnd, SW_SHOW);
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	_this->CreateDeviceAndSwapChain();
	_this->InitalizeImGui();

}

Window& Window::Get() {
	return *_this;
}

WPARAM Window::MessageLoop(std::function<void()> func) {

	MSG msg;
	INT gResult;

	while ((gResult = GetMessage(&msg, hWnd, 0, 0)) > 0) {

		TranslateMessage(&msg);
		DispatchMessage(&msg);
		func();
	}

	if (gResult == -1) {
		return -1;
	}
	else {
		return msg.wParam;
	}
}

LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	switch (msg)
	{
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_CLOSE:
		::PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
