#include "Camera.h"
#include "Window.h"

Camera::Camera() {
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

void Camera::Bind() {
	DirectX::XMMATRIX Translation = DirectX::XMMatrixTranslation(translate.x, translate.y, translate.z);
	DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(rotation.pitch), DirectX::XMConvertToRadians(rotation.yaw), DirectX::XMConvertToRadians(rotation.roll));
	DirectX::XMMATRIX Proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90), 16 / 9.f, 0.5f, 100.0f);

	Projection =
		//DirectX::XMMatrixScaling(0.5, 0.5, 0.5) *

		// Model

		// View 
		DirectX::XMMatrixInverse(
			nullptr, Translation * Rotation
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
