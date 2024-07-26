#pragma once
#include <Windows.h>
#include <wrl.h>
#include <dxgi.h>
#include <d3d11.h>

class Graphics
{
public:
	Graphics(HWND& hWnd, int windowWidth, int windowHeight);
	~Graphics() = default;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	void Draw();

private:
	void CheckHR(HRESULT hr);
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView;
};

