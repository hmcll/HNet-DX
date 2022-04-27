#pragma once


#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>

#include <vector>
#include <fstream>
#include <iostream>
#include <functional>
#include <memory>
#include <queue>
#include <unordered_map>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment(lib,"D3DCompiler.lib")

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;
};