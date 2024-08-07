#pragma once
#include "ThrowMacros.h"
#include "Bindable.h"

template<typename Structure>
class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer(Graphics& graphics, const Structure& data, UINT slot = 0u):
		slot(slot)
	{
		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.ByteWidth = sizeof(data);
		D3D11_SUBRESOURCE_DATA bufferData = {};
		bufferData.pSysMem = &data;

		CHECK_HR(GetDevice(graphics)->CreateBuffer(&bufferDesc, &bufferData, &constantBuffer));
	}
	void Update(Graphics& graphics, const Structure& data)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		CHECK_HR(GetContext(graphics)->Map(constantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedSubresource));
		memcpy(mappedSubresource.pData, &data, sizeof(data));
		GetContext(graphics)->Unmap(constantBuffer.Get(), 0u);
	}
	void Bind(Graphics& graphics) noexcept
	{
		GetContext(graphics)->PSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf());
	}
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	UINT slot;
};