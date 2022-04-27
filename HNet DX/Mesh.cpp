#include "Mesh.h"
#include "Material.h"

#include "ofbx.h"

int Mesh::LoadMesh(std::string path, Mesh& obj) {

	std::ifstream ifs(path, std::ios::binary);

	ifs.seekg(0, std::ios_base::end);
	std::size_t size = ifs.tellg();
	ifs.seekg(0, std::ios_base::beg);
	char* data = new char[size];
	ifs.read(data, size);
	ofbx::IScene* g_scene = ofbx::load((ofbx::u8*)data, size, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);


	if (!g_scene) {
		std::cerr << ofbx::getError() << std::endl;
	}

	auto& device = Window::Get().device;
	obj.device = device.Get();
	obj.context = Window::Get().context.Get();

	int mesh_count = g_scene->getMeshCount();
	int vertexID = 0;
	for (int i = 0; i < mesh_count; i++)
	{
		const ofbx::Mesh& mesh = *g_scene->getMesh(i);
		const ofbx::Geometry& geom = *mesh.getGeometry();

		int indexCount = geom.getIndexCount();
		int vertexCount = geom.getVertexCount();

		unsigned int* baseIndices = new unsigned int[indexCount];
		const int* faceIndices = geom.getFaceIndices();

		const ofbx::Vec3* Normals = geom.getNormals();
		const ofbx::Vec3* Verticies = geom.getVertices();
		const ofbx::Vec2* UVs = geom.getUVs();
		
		for (int i = 0; i < vertexCount; i++) {
			Vertex in;
			in.Position.x = Verticies[i].x;
			in.Position.y = Verticies[i].y;
			in.Position.z = Verticies[i].z;
			in.Normal.x = Normals[i].x;
			in.Normal.y = Normals[i].y;
			in.Normal.z = Normals[i].z;
			in.TexCoord.x = UVs[i].x;
			in.TexCoord.y = UVs[i].y;
			obj.verticies.push_back(in);
		}

		for (int i = 0; i < indexCount; i++) {
			if (faceIndices[i] >= 0) {
				baseIndices[i]  = faceIndices[i];
			}
			else {
				baseIndices[i] = -faceIndices[i] -1;
			}
		}

		::Microsoft::WRL::ComPtr < ID3D11Buffer> IndexBuffer;

		D3D11_BUFFER_DESC IndexBufferDesc = {};
		IndexBufferDesc.ByteWidth = sizeof(unsigned int) * indexCount;
		IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		IndexBufferDesc.CPUAccessFlags = 0;
		IndexBufferDesc.MiscFlags = 0;
		IndexBufferDesc.StructureByteStride = sizeof(unsigned int);

		D3D11_SUBRESOURCE_DATA IndexBuffersubresdata = {};
		IndexBuffersubresdata.pSysMem = baseIndices;

		device->CreateBuffer(&IndexBufferDesc, &IndexBuffersubresdata, &IndexBuffer);
		obj.Indices.push_back(baseIndices);
		obj.IndicesSize.push_back(indexCount);
		obj.IndexBuffers.push_back(std::move(IndexBuffer));

		vertexID += vertexCount;
	}


	D3D11_BUFFER_DESC bufferdesc = {};
	bufferdesc.ByteWidth = sizeof(Vertex) * obj.verticies.size();
	bufferdesc.Usage = D3D11_USAGE_DEFAULT;
	bufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferdesc.CPUAccessFlags = 0;
	bufferdesc.MiscFlags = 0;
	bufferdesc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA subresdata = {};
	subresdata.pSysMem = obj.verticies.data();

	device->CreateBuffer(&bufferdesc, &subresdata, &obj.VertexBuffer);


	return 0;
}

void Mesh::setMaterial(Material* mat) {
	material = mat;
}

void Mesh::Render() {
	context->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &poffset);
	if (RenderAll) {
		for (int i = 0; i < Indices.size(); i++) {
			material->UseMaterial();
			context->IASetIndexBuffer(IndexBuffers[i].Get(), DXGI_FORMAT_R32_UINT, 0);
			context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->DrawIndexed(IndicesSize[i], 0, 0);

		}
	}
	material->UseMaterial();
	context->IASetIndexBuffer(IndexBuffers[id % Indices.size()].Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(IndicesSize[id % Indices.size()], 0, 0);


}

Mesh::~Mesh() {
	for (auto* indices : Indices) {
		delete[] indices;
	}
}
