#pragma once
#include "Bindable.h"
#include <wrl\client.h>

class Rasterizer : public Bindable
{
public:
	Rasterizer(Graphics& graphics, const D3D11_CULL_MODE cullMode);
	void Bind(Graphics& graphics) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
};