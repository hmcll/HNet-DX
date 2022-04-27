#pragma once
#include "Window.h"

class Mesh {
public:
	std::vector < Vertex> verticies;
	::Microsoft::WRL::ComPtr < ID3D11Buffer> VertexBuffer;
	std::vector<unsigned int* > Indices;
	std::vector< size_t> IndicesSize;
	std::vector< ::Microsoft::WRL::ComPtr <ID3D11Buffer>> IndexBuffers;

	ID3D11Device* device;
	ID3D11DeviceContext* context;


	const UINT stride = sizeof(Vertex);
	const UINT poffset = 0;

	static int LoadMesh(std::string path, Mesh& obj);
	class Material* material;
	void setMaterial(Material* mat);

	int id = 0;
	bool RenderAll;
	void Render();
	~Mesh();
};
