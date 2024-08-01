#pragma once
#include <DirectXMath.h>

class Camera
{
public:
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void Move(DirectX::XMVECTOR moveVector) noexcept;
	void Rotate(DirectX::XMVECTOR rotateVector) noexcept;
private:
	DirectX::XMVECTOR position = { 0.f, 0.f, -10.f };
	DirectX::XMVECTOR rotation = { 0.f, 0.f, 0.f };
};

