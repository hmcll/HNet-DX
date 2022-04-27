#include "TextureManager.h"
#include "Window.h"
#include "WICTextureLoader.h"

std::unique_ptr<TextureManager> TextureManager::_this = nullptr;

TextureManager& TextureManager::Get() {
	if (!_this) {
		_this = std::make_unique < TextureManager>();
		_this->device = Window::Get().device.Get();
	}
	return *_this;
}

void TextureManager::LoadTextureFromFile(LPCWSTR FileName, size_t* ResourceID, size_t* ShaderResourceID) {

	::Microsoft::WRL::ComPtr < ID3D11Resource> Texture_resource;
	::Microsoft::WRL::ComPtr < ID3D11ShaderResourceView> Texture_ShaderResource;

	DirectX::CreateWICTextureFromFile(device, FileName, &Texture_resource, &Texture_ShaderResource);
	::Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
	Texture_resource.As(&tex);
	D3D11_TEXTURE2D_DESC texture_desc;
	tex->GetDesc(&texture_desc);
	float ratio = texture_desc.Width / float(texture_desc.Height);

	_Texture_ResourceStack.push_back(std::move(Texture_resource));
	_Texture_ShaderResourceStack.push_back(std::move(Texture_ShaderResource));
	if (ResourceID)
		*ResourceID = _Texture_ResourceStack.size() - 1;
	if (ShaderResourceID)
		*ShaderResourceID = _Texture_ShaderResourceStack.size() - 1;
}

size_t TextureManager::createSampler() {

	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	::Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	device->CreateSamplerState(&samplerDesc, &sampler);
	_SamplerStateStack.push_back(std::move(sampler));
	return _SamplerStateStack.size() - 1;

}

ID3D11Resource* TextureManager::GetResource(size_t ResourceID) {
	return _Texture_ResourceStack[ResourceID].Get();
}

ID3D11ShaderResourceView* TextureManager::GetShaderResource(size_t ShaderResourceID) {
	return _Texture_ShaderResourceStack[ShaderResourceID].Get();
}

ID3D11SamplerState* TextureManager::GetSampler(size_t SamplerID) {
	return _SamplerStateStack[SamplerID].Get();
}
