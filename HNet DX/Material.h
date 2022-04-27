#pragma once
#include "Include.h"

template<>
struct std::less<class Material*> {
	// Declaration of the less operation
	bool operator() (const Material* x,
		const Material* y)
		const;

	// Type of first parameter
	typedef Material* first_argument_type;

	// Type of second parameter
	typedef Material* second_argument_type;

	// The result is returned
	// as bool type
	typedef bool result_type;
};

class Material {
	Material() = default;
	static std::priority_queue < Material*, std::vector<Material*>, std::less<Material*>> MaterialQueue;
	static std::vector<Material*> MaterialStack;
	static size_t MatRenderPtr;
	ID3D11DeviceContext* context;
	size_t MatID;
	size_t VertexShaderID, PixelShaderID, InputLayoutID;

	std::vector<ID3D11ShaderResourceView*> ShaderResourceStack;
	std::vector<ID3D11SamplerState*> SamplerStack;
	friend std::less<class Material*>;

public:
	static Material* CreateMaterial(size_t _VertexShaderID, size_t _PixelShaderID, size_t _InputLayoutID);

	void AddTexture(size_t _ShaderResourceID, size_t _SamplerID);
	void UseMaterial();
	static void RegisterMaterials();
	static void DestroyMaterials();
	static void StartRendering();
};
