#pragma once
#include "Include.h"

class Camera {
public:
	DirectX::XMMATRIX Projection;
	::Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
	struct
	{
		float  pitch, yaw, roll;
	} rotation;
	struct
	{
		float x, y, z = -3;
	}translate;
	Camera();
	void Bind();
};