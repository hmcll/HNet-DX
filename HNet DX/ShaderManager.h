#include "Include.h"

class ShaderManager {
	::Microsoft::WRL::ComPtr< ID3DBlob> _FBlob;
	std::vector< ::Microsoft::WRL::ComPtr <ID3D11VertexShader>> _VertexShaderStack;
	std::vector< ::Microsoft::WRL::ComPtr <ID3D11InputLayout>> _InputLayoutStack;
	std::vector< ::Microsoft::WRL::ComPtr <ID3D11PixelShader>> _PixelShaderStack;

	static std::unique_ptr<ShaderManager> _this;
public:
	static ShaderManager& Get();
	size_t LoadVertexShaderFromFile(LPCWSTR FileName);

	void LoadVertexShaderFromFile(LPCWSTR FileName, size_t& VertexShaderID);
	void LoadVertexShaderFromFileAndSetInputLayout(LPCWSTR FileName, const D3D11_INPUT_ELEMENT_DESC* InputDesc, size_t InputDescSize, size_t& VertexShaderID, size_t& InputLayoutID);
	size_t LoadPixelShaderFromFile(LPCWSTR FileName);
	void LoadPixelShaderFromFile(LPCWSTR FileName, size_t& PixelShaderID);
	ID3D11InputLayout* GetInputLayout(size_t InputLayoutID);
	ID3D11VertexShader* GetVertexShader(size_t VertexShaderID);
	ID3D11PixelShader* GetPixelShader(size_t PixelShaderID);
};