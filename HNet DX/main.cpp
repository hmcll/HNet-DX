#define _XM_NO_INTRINSICS_

#include "Window.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment(lib,"D3DCompiler.lib")


class ShaderManager {
	::Microsoft::WRL::ComPtr< ID3DBlob> _FBlob;
	std::vector< ::Microsoft::WRL::ComPtr <ID3D11VertexShader>> _VertexShaderStack;
	std::vector< ::Microsoft::WRL::ComPtr <ID3D11InputLayout>> _InputLayoutStack;
	std::vector< ::Microsoft::WRL::ComPtr <ID3D11PixelShader>> _PixelShaderStack;

	static std::unique_ptr<ShaderManager> _this;
public:
	static ShaderManager& Get() {
		if (!_this) _this = std::make_unique < ShaderManager>();
		return *_this;
	}
	size_t LoadVertexShaderFromFile(LPCWSTR FileName) {
		::Microsoft::WRL::ComPtr <ID3D11VertexShader> VS;
		D3DReadFileToBlob(FileName, &_FBlob);
		Window::Get().device->CreateVertexShader(_FBlob->GetBufferPointer(), _FBlob->GetBufferSize(), nullptr, &VS);
		_VertexShaderStack.push_back(std::move(VS));
		return _VertexShaderStack.size() - 1;
	}

	void LoadVertexShaderFromFile(LPCWSTR FileName, size_t& VertexShaderID) {
		VertexShaderID = LoadVertexShaderFromFile(FileName);

	}
	void LoadVertexShaderFromFileAndSetInputLayout(LPCWSTR FileName, const D3D11_INPUT_ELEMENT_DESC* InputDesc, size_t InputDescSize, size_t& VertexShaderID, size_t& InputLayoutID) {
		VertexShaderID = LoadVertexShaderFromFile(FileName);
		::Microsoft::WRL::ComPtr <ID3D11InputLayout>InputLayout;
		Window::Get().device->CreateInputLayout(InputDesc, InputDescSize, _FBlob->GetBufferPointer(), _FBlob->GetBufferSize(), &InputLayout);
		_InputLayoutStack.push_back(std::move(InputLayout));
		InputLayoutID = _InputLayoutStack.size() - 1;
	}
	size_t LoadPixelShaderFromFile(LPCWSTR FileName) {
		::Microsoft::WRL::ComPtr <ID3D11PixelShader> PS;
		D3DReadFileToBlob(L"pixelShader.cso", &_FBlob);
		Window::Get().device->CreatePixelShader(_FBlob->GetBufferPointer(), _FBlob->GetBufferSize(), nullptr, &PS);
		_PixelShaderStack.push_back(std::move(PS));
		return _PixelShaderStack.size() - 1;
	}
	void LoadPixelShaderFromFile(LPCWSTR FileName, size_t& PixelShaderID) {

		PixelShaderID = LoadPixelShaderFromFile(FileName);
	}
	ID3D11InputLayout* GetInputLayout(size_t InputLayoutID) {
		return _InputLayoutStack[InputLayoutID].Get();
	}
	ID3D11VertexShader* GetVertexShader(size_t VertexShaderID) {
		return _VertexShaderStack[VertexShaderID].Get();
	}
	ID3D11PixelShader* GetPixelShader(size_t PixelShaderID) {
		return _PixelShaderStack[PixelShaderID].Get();
	}
};
class TextureManager {
	std::vector<::Microsoft::WRL::ComPtr < ID3D11Resource>> _Texture_ResourceStack;
	std::vector<::Microsoft::WRL::ComPtr < ID3D11ShaderResourceView>> _Texture_ShaderResourceStack;
	std::vector< ::Microsoft::WRL::ComPtr <ID3D11SamplerState>> _SamplerStateStack;

	static std::unique_ptr<TextureManager> _this;
	ID3D11Device* device;

public:
	static TextureManager& Get() {
		if (!_this) {
			_this = std::make_unique < TextureManager>();
			_this->device = Window::Get().device.Get();
		}
		return *_this;
	}
	void LoadTextureFromFile(LPCWSTR FileName, size_t* ResourceID, size_t* ShaderResourceID) {

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
	size_t createSampler() {

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

	ID3D11Resource* GetResource(size_t ResourceID) {
		return _Texture_ResourceStack[ResourceID].Get();
	}
	ID3D11ShaderResourceView* GetShaderResource(size_t ShaderResourceID) {
		return _Texture_ShaderResourceStack[ShaderResourceID].Get();
	}
	ID3D11SamplerState* GetSampler(size_t SamplerID) {
		return _SamplerStateStack[SamplerID].Get();
	}

};
std::unique_ptr<ShaderManager> ShaderManager::_this = nullptr;
std::unique_ptr<TextureManager> TextureManager::_this = nullptr;

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;
};

const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
	{"Position", 0 ,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"Normal", 0 ,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"TextureCoordinate", 0 ,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0}
};

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
	static Material* CreateMaterial(size_t _VertexShaderID, size_t _PixelShaderID, size_t _InputLayoutID) {
		Material* mat = new Material();
		mat->VertexShaderID = _VertexShaderID;
		mat->PixelShaderID = _PixelShaderID;
		mat->InputLayoutID = _InputLayoutID;

		mat->context = Window::Get().context.Get();
		MaterialQueue.push(mat);
		return mat;
	}

	void AddTexture(size_t _ShaderResourceID, size_t _SamplerID) {
		ShaderResourceStack.push_back(TextureManager::Get().GetShaderResource(_ShaderResourceID));
		SamplerStack.push_back(TextureManager::Get().GetSampler(_SamplerID));
	}
	void UseMaterial() {
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
	static void RegisterMaterials() {
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
	static void DestroyMaterials() {
		for (Material* m : MaterialStack) {
			delete m;
		}
	}
	static void StartRendering() {
		MatRenderPtr = -1;
	}
};

std::priority_queue < Material*> Material::MaterialQueue = std::priority_queue < Material*>();
std::vector<Material*> Material::MaterialStack = std::vector<Material*>();
size_t Material::MatRenderPtr = 0;;

bool std::less<Material*>::operator() (const Material* x,
	const Material* y)
	const
{
	return x->VertexShaderID < y->VertexShaderID;
}
template<>
struct std::hash<Vertex> {
	const static size_t len = sizeof(Vertex);
	static size_t c;
	unsigned int MurmurHash2(const void* key, int len, unsigned int seed)const
	{
		// 'm' and 'r' are mixing constants generated offline.
		// They're not really 'magic', they just happen to work well.

		const unsigned int m = 0x5bd1e995;
		const int r = 24;

		// Initialize the hash to a 'random' value

		unsigned int h = seed ^ len;

		// Mix 4 bytes at a time into the hash

		const unsigned char* data = (const unsigned char*)key;

		while (len >= 4)
		{
			unsigned int k = *(unsigned int*)data;

			k *= m;
			k ^= k >> r;
			k *= m;

			h *= m;
			h ^= k;

			data += 4;
			len -= 4;
		}

		// Handle the last few bytes of the input array

		switch (len)
		{
		case 3: h ^= data[2] << 16;
		case 2: h ^= data[1] << 8;
		case 1: h ^= data[0];
			h *= m;
		};

		// Do a few final mixes of the hash to ensure the last few
		// bytes are well-incorporated.

		h ^= h >> 13;
		h *= m;
		h ^= h >> 15;

		return h;
	}
	size_t operator()(const Vertex& key) const {
		return MurmurHash2(&key, len, c);
	}

};
size_t std::hash<Vertex>::c = rand();
template<>
struct std::equal_to<Vertex> {
	constexpr bool operator()(const Vertex& lhs, const Vertex& rhs) const {
		return  lhs.Normal.x == rhs.Normal.x &&
			lhs.Normal.y == rhs.Normal.y &&
			lhs.Normal.z == rhs.Normal.z &&
			lhs.Position.x == rhs.Position.x &&
			lhs.Position.y == rhs.Position.y &&
			lhs.Position.z == rhs.Position.z &&
			lhs.TexCoord.x == rhs.TexCoord.x &&
			lhs.TexCoord.y == rhs.TexCoord.y;
	}

};

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

	static int LoadMesh(std::string path, Mesh& obj) {

		tinyobj::attrib_t attrib;

		std::vector<tinyobj::shape_t> shapes;

		std::string warn;
		std::string err;
		std::ifstream ifs(path.c_str());

		if (ifs.fail()) {
			std::cerr << "file not found." << std::endl;
			return -1;
		}
		bool ret = tinyobj::LoadObj(&attrib, &shapes, nullptr, &warn, &err, &ifs);
		
		if (!warn.empty()) {
			std::cout << "WARN: " << warn << std::endl;
		}

		if (!err.empty()) {
			std::cerr << err << std::endl;
		}

		if (!ret) {
			std::cerr << "Failed to parse .obj" << std::endl; return -1;
		}

		auto& device = Window::Get().device;
		obj.device = device.Get();
		obj.context = Window::Get().context.Get();

		for (auto shape : shapes) {
			unsigned int* baseIndices = new unsigned int[shape.mesh.indices.size()];
			for (int i = 0; i < shape.mesh.indices.size(); i++) {
				Vertex in;
				in.Position.x = attrib.vertices[shape.mesh.indices[i].vertex_index * 3.0];
				in.Position.y = attrib.vertices[shape.mesh.indices[i].vertex_index * 3.0 + 1];
				in.Position.z = attrib.vertices[shape.mesh.indices[i].vertex_index * 3.0 + 2];
				in.Normal.x = attrib.normals[shape.mesh.indices[i].normal_index * 3.0];
				in.Normal.y = attrib.normals[shape.mesh.indices[i].normal_index * 3.0 + 1];
				in.Normal.z = attrib.normals[shape.mesh.indices[i].normal_index * 3.0 + 2];
				in.TexCoord.x = attrib.texcoords[shape.mesh.indices[i].texcoord_index * 2.0];
				in.TexCoord.y = attrib.texcoords[shape.mesh.indices[i].texcoord_index * 2.0 + 1];
				baseIndices[i] = obj.verticies.size();
				obj.verticies.push_back(in);
			}
			::Microsoft::WRL::ComPtr < ID3D11Buffer> IndexBuffer;

			D3D11_BUFFER_DESC IndexBufferDesc = {};
			IndexBufferDesc.ByteWidth = sizeof(unsigned int)* shape.mesh.indices.size();
			IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			IndexBufferDesc.CPUAccessFlags = 0;
			IndexBufferDesc.MiscFlags = 0;
			IndexBufferDesc.StructureByteStride = sizeof(unsigned int);

			D3D11_SUBRESOURCE_DATA IndexBuffersubresdata = {};
			IndexBuffersubresdata.pSysMem = baseIndices;

			device->CreateBuffer(&IndexBufferDesc, &IndexBuffersubresdata, &IndexBuffer);
			obj.Indices.push_back(baseIndices);
			obj.IndicesSize.push_back(shape.mesh.indices.size());
			obj.IndexBuffers.push_back(std::move(IndexBuffer));
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
	Material* material;
	void setMaterial(Material* mat) {
		material = mat;
	}

	int id = 0;
	bool RenderAll;
	void Render() {
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
			context->IASetIndexBuffer(IndexBuffers[id% Indices.size()].Get(), DXGI_FORMAT_R32_UINT, 0);
			context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->DrawIndexed(IndicesSize[id%Indices.size()],0, 0);


	}
	~Mesh() {
		for (auto* indices : Indices) {
			delete[] indices;
		}
	}
};

class Camera {
public:
	DirectX::XMMATRIX Projection;
	::Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
	struct
	{
		float  pitch, yaw , roll;
	} rotation;
	struct
	{
		float x, y, z=-3;
	}translate;
	Camera() {
		Projection = DirectX::XMMatrixIdentity();
		D3D11_BUFFER_DESC ConstantBufferDesc;
		ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		ConstantBufferDesc.MiscFlags = 0u;
		ConstantBufferDesc.ByteWidth = sizeof(DirectX::XMMATRIX);
		ConstantBufferDesc.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA ConstantBufferSubResourceData = {};
		ConstantBufferSubResourceData.pSysMem = &Projection;
		Window::Get().device->CreateBuffer(&ConstantBufferDesc, &ConstantBufferSubResourceData, &ConstantBuffer);
	}
	void Bind() {
		DirectX::XMMATRIX Translation  = DirectX::XMMatrixTranslation(translate.x, translate.y, translate.z);
		DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(rotation.pitch), DirectX::XMConvertToRadians(rotation.yaw), DirectX::XMConvertToRadians(rotation.roll));
		DirectX::XMMATRIX Proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90), 16 / 9.f, 0.5f, 100.0f);
		
		Projection = 
			//DirectX::XMMatrixScaling(0.5, 0.5, 0.5) *

			// Model
			
			// View 
			DirectX::XMMatrixInverse(
				nullptr,Translation * Rotation
			)
			// P
				 * Proj
			//* DirectX::XMMatrixTranslation(0,0,5)
			
		/*DirectX::XMMATRIX Proj = */;

		D3D11_MAPPED_SUBRESOURCE subres;
		Window::Get().context->Map(ConstantBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &subres);
		*((DirectX::XMMATRIX*)subres.pData) = Projection;
		Window::Get().context->Unmap(ConstantBuffer.Get(), 0);
		Window::Get().context->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	}
};



int CALLBACK  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	Window::Initalize(hInstance, L"Main");

	size_t VertexShaderID, PixelShaderID, InputLayoutID;

	ShaderManager::Get().LoadVertexShaderFromFileAndSetInputLayout(L"vertexShader.cso", inputElementDesc, std::size(inputElementDesc), VertexShaderID, InputLayoutID);

	ShaderManager::Get().LoadPixelShaderFromFile(L"pixelShader.cso", PixelShaderID);

	Material* mat = Material::CreateMaterial(VertexShaderID, PixelShaderID, InputLayoutID);
	size_t TextureShaderResource;
	TextureManager::Get().LoadTextureFromFile(L"in.jpg", nullptr, &TextureShaderResource);
	mat->AddTexture(TextureManager::Get().createSampler(), TextureShaderResource);

	Material::RegisterMaterials();

	Mesh monkey;
	Mesh::LoadMesh("monkey.obj", monkey);
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