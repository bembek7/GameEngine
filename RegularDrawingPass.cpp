#include "RegularDrawingPass.h"
#include "Graphics.h"
#include "Camera.h"
#include "Actor.h"
#include "ScissorRectangle.h"
#include "Viewport.h"

RegularDrawingPass::RegularDrawingPass(Graphics& graphics)
{
	const float windowWidth = graphics.GetWindowWidth();
	const float windowHeight = graphics.GetWindowHeight();

	bindables.push_back(std::make_unique<ScissorRectangle>());
	bindables.push_back(std::make_unique<Viewport>(windowWidth, windowHeight));

	DirectX::XMMATRIX reverseZ =
	{
		1.0f, 0.0f,  0.0f, 0.0f,
		0.0f, 1.0f,  0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f,  1.0f, 1.0f
	};
	DirectX::XMStoreFloat4x4(&projection, DirectX::XMMatrixPerspectiveLH(1.0f, windowHeight / windowWidth, 0.5f, 200.0f) * reverseZ);
}

void RegularDrawingPass::Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors, const std::vector<Light*>& lights, const Camera* const mainCamera)
{
	Pass::Execute(graphics);
	graphics.SetCamera(mainCamera->GetMatrix());
	for (auto& actor : actors)
	{
		actor->Draw(graphics, lights);
	}
}