#include "Material.h"
#include "Window.h"
#include "ShaderManager.h"
#include "TextureManager.h"

Material* Material::CreateMaterial(size_t _VertexShaderID, size_t _PixelShaderID, size_t _InputLayoutID) {
	Material* mat = new Material();
	mat->VertexShaderID = _VertexShaderID;
	mat->PixelShaderID = _PixelShaderID;
	mat->InputLayoutID = _InputLayoutID;

	mat->context = Window::Get().context.Get();
	MaterialQueue.push(mat);
	return mat;
}

void Material::AddTexture(size_t _ShaderResourceID, size_t _SamplerID) {
	ShaderResourceStack.push_back(TextureManager::Get().GetShaderResource(_ShaderResourceID));
	SamplerStack.push_back(TextureManager::Get().GetSampler(_SamplerID));
}

void Material::UseMaterial() {
	if (MatRenderPtr == MatID) {
		return;
	}
	context->IASetInputLayout(ShaderManager::Get().GetInputLayout(InputLayoutID));
	context->VSSetShader(ShaderManager::Get().GetVertexShader(VertexShaderID), nullptr, 0);
	context->PSSetShader(ShaderManager::Get().GetPixelShader(PixelShaderID), nullptr, 0);
	if (!SamplerStack.empty()) {
		context->PSSetSamplers(0, SamplerStack.size(), SamplerStack.data());
		context->PSSetShaderResources(0, ShaderResourceStack.size(), ShaderResourceStack.data());
	}
	MatRenderPtr = MatID;

}

void Material::RegisterMaterials() {
	MaterialStack.clear();
	MaterialStack.resize(MaterialQueue.size() + 1);
	size_t ID = 0;
	while (!MaterialQueue.empty()) {
		Material* m = MaterialQueue.top();
		MaterialStack.push_back(m);
		m->MatID = ID++;
		MaterialQueue.pop();
	}
}

void Material::DestroyMaterials() {
	for (Material* m : MaterialStack) {
		delete m;
	}
}

void Material::StartRendering() {
	MatRenderPtr = -1;
}

bool std::less<Material*>::operator() (const Material* x,
	const Material* y)
	const
{
	return x->VertexShaderID < y->VertexShaderID;
}

std::priority_queue < Material*> Material::MaterialQueue = std::priority_queue < Material*>();
std::vector<Material*> Material::MaterialStack = std::vector<Material*>();
size_t Material::MatRenderPtr = 0;;
