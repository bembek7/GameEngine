#pragma once
#include "Light.h"
class SpotLight : public Light
{
	friend class Gui;
public:
	SpotLight(Graphics& graphics, const std::string& fileName, const std::string& actorName = "Spot Light");
	virtual void Update(Graphics& graphics) override;

protected:
	virtual void RenderActorDetails(Gui& gui) override;
private:

	struct LightBuffer
	{
		DirectX::XMFLOAT3 diffuseColor = { 1.0f, 1.0f, 1.0f };
		float diffuseIntensity = 1.0f;
		DirectX::XMFLOAT3 ambient = { 0.05f, 0.05f, 0.05f };
		float specularIntensity = 0.6f;
		DirectX::XMFLOAT3 lightViewLocation = { 0.f, 0.f, 0.f };
		float spotPower = 1.0f;
		DirectX::XMFLOAT3 lightDirection = { 0.f, 0.f, 0.f };
		float attenuationConst = 1.0f;
		float attenuationLin = 0.045f;
		float attenuationQuad = 0.0020f;
		char padding[8];
	} lightBuffer;
};