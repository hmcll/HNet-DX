#include "ShaderManager.h"
#include "Window.h"

std::unique_ptr<ShaderManager> ShaderManager::_this = nullptr;

ShaderManager& ShaderManager::Get() {
	if (!_this) _this = std::make_unique < ShaderManager>();
	return *_this;
}

size_t ShaderManager::LoadVertexShaderFromFile(LPCWSTR FileName) {
	::Microsoft::WRL::ComPtr <ID3D11VertexShader> VS;
	D3DReadFileToBlob(FileName, &_FBlob);
	Window::Get().device->CreateVertexShader(_FBlob->GetBufferPointer(), _FBlob->GetBufferSize(), nullptr, &VS);
	_VertexShaderStack.push_back(std::move(VS));
	return _VertexShaderStack.size() - 1;
}

void ShaderManager::LoadVertexShaderFromFile(LPCWSTR FileName, size_t& VertexShaderID) {
	VertexShaderID = LoadVertexShaderFromFile(FileName);

}

void ShaderManager::LoadVertexShaderFromFileAndSetInputLayout(LPCWSTR FileName, const D3D11_INPUT_ELEMENT_DESC* InputDesc, size_t InputDescSize, size_t& VertexShaderID, size_t& InputLayoutID) {
	VertexShaderID = LoadVertexShaderFromFile(FileName);
	::Microsoft::WRL::ComPtr <ID3D11InputLayout>InputLayout;
	Window::Get().device->CreateInputLayout(InputDesc, InputDescSize, _FBlob->GetBufferPointer(), _FBlob->GetBufferSize(), &InputLayout);
	_InputLayoutStack.push_back(std::move(InputLayout));
	InputLayoutID = _InputLayoutStack.size() - 1;
}

size_t ShaderManager::LoadPixelShaderFromFile(LPCWSTR FileName) {
	::Microsoft::WRL::ComPtr <ID3D11PixelShader> PS;
	D3DReadFileToBlob(FileName, &_FBlob);
	Window::Get().device->CreatePixelShader(_FBlob->GetBufferPointer(), _FBlob->GetBufferSize(), nullptr, &PS);
	_PixelShaderStack.push_back(std::move(PS));
	return _PixelShaderStack.size() - 1;
}

void ShaderManager::LoadPixelShaderFromFile(LPCWSTR FileName, size_t& PixelShaderID) {

	PixelShaderID = LoadPixelShaderFromFile(FileName);
}

ID3D11InputLayout* ShaderManager::GetInputLayout(size_t InputLayoutID) {
	return _InputLayoutStack[InputLayoutID].Get();
}

ID3D11VertexShader* ShaderManager::GetVertexShader(size_t VertexShaderID) {
	return _VertexShaderStack[VertexShaderID].Get();
}

ID3D11PixelShader* ShaderManager::GetPixelShader(size_t PixelShaderID) {
	return _PixelShaderStack[PixelShaderID].Get();
}
