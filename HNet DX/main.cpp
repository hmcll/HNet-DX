#define _XM_NO_INTRINSICS_

#include "Window.h"
#include "Material.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Mesh.h"
#include "Camera.h"


const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
	{"Position", 0 ,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"Normal", 0 ,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"TextureCoordinate", 0 ,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0}
};


int CALLBACK  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	Window::Initalize(hInstance, L"Main");

	size_t VertexShaderID, PixelShaderID, InputLayoutID;

	ShaderManager::Get().LoadVertexShaderFromFileAndSetInputLayout(L"vertexShader.cso", inputElementDesc, std::size(inputElementDesc), VertexShaderID, InputLayoutID);

	ShaderManager::Get().LoadPixelShaderFromFile(L"pixelShader.cso", PixelShaderID);

	Material* mat = Material::CreateMaterial(VertexShaderID, PixelShaderID, InputLayoutID);
	size_t TextureShaderResource;
	TextureManager::Get().LoadTextureFromFile(L"Res\\in.jpg", nullptr, &TextureShaderResource);
	mat->AddTexture(TextureManager::Get().createSampler(), TextureShaderResource);

	Material::RegisterMaterials();

	Mesh monkey;
	Mesh::LoadMesh("Res\\CO.fbx", monkey);
	monkey.setMaterial(mat);

	auto& context = Window::Get().context;
	auto& swapchain = Window::Get().swapchain;
	auto& BackBuffer_Target = Window::Get().BackBuffer_Target;

	Camera camera;
	
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	DepthStencilDesc. DepthEnable = TRUE;
	DepthStencilDesc. DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc. DepthFunc = D3D11_COMPARISON_LESS;
	DepthStencilDesc. StencilEnable = FALSE;
	DepthStencilDesc. StencilReadMask = 0;
	DepthStencilDesc. StencilWriteMask = 0;
	DepthStencilDesc. FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc. FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc. FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc. FrontFace.StencilFunc = D3D11_COMPARISON_NEVER;
	DepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	::Microsoft::WRL::ComPtr < ID3D11DepthStencilState> DepthStencilState;
	Window::Get().device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState);
	::Microsoft::WRL::ComPtr < ID3D11DepthStencilView> DepthStencilView;
	
	::Microsoft::WRL::ComPtr < ID3D11Texture2D> DepthStencil;

	D3D11_TEXTURE2D_DESC DepthStencilTextureDesc;
	DepthStencilTextureDesc.Width = 1904;
	DepthStencilTextureDesc.Height = 1041;
	DepthStencilTextureDesc.MipLevels = 1;
	DepthStencilTextureDesc.ArraySize = 1;
	DepthStencilTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthStencilTextureDesc.SampleDesc.Count = 1;
	DepthStencilTextureDesc.SampleDesc.Quality = 0;
	DepthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilTextureDesc.CPUAccessFlags = 0;
	DepthStencilTextureDesc.MiscFlags = 0;
	Window::Get().device->CreateTexture2D(&DepthStencilTextureDesc, nullptr,&DepthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Flags = 0;
	DepthStencilViewDesc.Texture2D.MipSlice = 0u;

	Window::Get().device->CreateDepthStencilView(DepthStencil.Get(), &DepthStencilViewDesc,&DepthStencilView);

	::Microsoft::WRL::ComPtr < ID3D11RasterizerState> RasterState;

	D3D11_RASTERIZER_DESC RasterStateDesc;
	RasterStateDesc. FillMode = D3D11_FILL_SOLID;
	RasterStateDesc. CullMode = D3D11_CULL_NONE;
	RasterStateDesc. FrontCounterClockwise = TRUE;
	RasterStateDesc. DepthBias = 0;
	RasterStateDesc. DepthBiasClamp = 0;
	RasterStateDesc. SlopeScaledDepthBias = 0;
	RasterStateDesc. DepthClipEnable = TRUE;
	RasterStateDesc. ScissorEnable = FALSE;
	RasterStateDesc. MultisampleEnable = FALSE;
	RasterStateDesc. AntialiasedLineEnable = FALSE;
	Window::Get().device->CreateRasterizerState(&RasterStateDesc,&RasterState);

	auto ret = Window::Get().MessageLoop([&]() {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow();
		ImGui::SliderFloat3("Camera Rotation", &camera.rotation.pitch,-360, 360);
		ImGui::SliderFloat3("Camera Translate", &camera.translate.x, -10, 10);
		ImGui::Checkbox("Render All", &monkey.RenderAll);
		if(!monkey.RenderAll){
			ImGui::SameLine();
			ImGui::InputInt("id", &monkey.id);
		}
		FLOAT color[4] = { 0.2f,0.f,0.f,0.f };
		context->ClearRenderTargetView(BackBuffer_Target.Get(), color);
		context->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1, 0);
		Material::StartRendering();
		context->RSSetState(RasterState.Get());
		context->OMSetRenderTargets(1, Window::Get().BackBuffer_Target.GetAddressOf(), DepthStencilView.Get());
		context->RSSetViewports(1, &Window::Get().viewport);
		camera.Bind();
		monkey.Render();

		//con
		ImGui::Render();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swapchain->Present(1, 0);
		}
	);
	Material::DestroyMaterials();
	return ret;
}