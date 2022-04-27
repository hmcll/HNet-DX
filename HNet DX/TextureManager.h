#include "Include.h"

class TextureManager {
	std::vector<::Microsoft::WRL::ComPtr < ID3D11Resource>> _Texture_ResourceStack;
	std::vector<::Microsoft::WRL::ComPtr < ID3D11ShaderResourceView>> _Texture_ShaderResourceStack;
	std::vector< ::Microsoft::WRL::ComPtr <ID3D11SamplerState>> _SamplerStateStack;

	static std::unique_ptr<TextureManager> _this;
	ID3D11Device* device;

public:
	static TextureManager& Get();
	void LoadTextureFromFile(LPCWSTR FileName, size_t* ResourceID, size_t* ShaderResourceID);
	size_t createSampler();

	ID3D11Resource* GetResource(size_t ResourceID);
	ID3D11ShaderResourceView* GetShaderResource(size_t ShaderResourceID);
	ID3D11SamplerState* GetSampler(size_t SamplerID);

};