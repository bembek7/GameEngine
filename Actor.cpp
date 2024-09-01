#include "BetterWindows.h"
#include "Actor.h"
#include "imgui.h"
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <numbers>
#include "MeshComponent.h"

Actor::Actor(const std::string& actorName) :
	actorName(actorName)
{}

void Actor::AddActorScale(const DirectX::XMFLOAT3 scaleToAdd) noexcept
{
	rootComponent->AddRelativeScale(scaleToAdd);
}

void Actor::AddActorRotation(const DirectX::XMFLOAT3 rotationToAdd) noexcept
{
	rootComponent->AddRelativeRotation(rotationToAdd);
}

void Actor::AddActorLocation(const DirectX::XMFLOAT3 locationToAdd) noexcept
{
	rootComponent->AddRelativeLocation(locationToAdd);
}

void Actor::AddActorScale(const DirectX::XMVECTOR scaleToAdd) noexcept
{
	rootComponent->AddRelativeScale(scaleToAdd);
}

void Actor::AddActorRotation(const DirectX::XMVECTOR rotationToAdd) noexcept
{
	rootComponent->AddRelativeRotation(rotationToAdd);
}

void Actor::AddActorLocation(const DirectX::XMVECTOR locationToAdd) noexcept
{
	rootComponent->AddRelativeLocation(locationToAdd);
}

void Actor::SetActorScale(const DirectX::XMFLOAT3 newScale) noexcept
{
	rootComponent->SetRelativeScale(newScale);
}

void Actor::SetActorRotation(const DirectX::XMFLOAT3 newRotation) noexcept
{
	rootComponent->SetRelativeRotation(newRotation);
}

void Actor::SetActorLocation(const DirectX::XMFLOAT3 newLocation) noexcept
{
	rootComponent->SetRelativeLocation(newLocation);
}

void Actor::SetActorScale(const DirectX::XMVECTOR newScale) noexcept
{
	rootComponent->SetRelativeScale(newScale);
}

void Actor::SetActorRotation(const DirectX::XMVECTOR newRotation) noexcept
{
	rootComponent->SetRelativeRotation(newRotation);
}

void Actor::SetActorLocation(const DirectX::XMVECTOR newLocation) noexcept
{
	rootComponent->SetRelativeLocation(newLocation);
}

void Actor::SetActorTransform(const DirectX::XMFLOAT3 newLocation, const DirectX::XMFLOAT3 newRotation, const DirectX::XMFLOAT3 newScale) noexcept
{
	SetActorLocation(newLocation);
	SetActorRotation(newRotation);
	SetActorScale(newScale);
}

DirectX::XMFLOAT3 Actor::GetActorScale() const noexcept
{
	return rootComponent->GetRelativeScale();
}

DirectX::XMFLOAT3 Actor::GetActorRotation() const noexcept
{
	return rootComponent->GetRelativeRotation();
}

DirectX::XMFLOAT3 Actor::GetActorLocation() const noexcept
{
	return rootComponent->GetRelativeLocation();
}

DirectX::XMVECTOR Actor::GetActorScaleVector() const noexcept
{
	return rootComponent->GetRelativeScaleVector();
}

DirectX::XMVECTOR Actor::GetActorRotationVector() const noexcept
{
	return rootComponent->GetRelativeRotationVector();
}

DirectX::XMVECTOR Actor::GetActorLocationVector() const noexcept
{
	return rootComponent->GetRelativeLocationVector();
}

void Actor::Draw(Graphics& graphics)
{
	if (rootComponent)
	{
		rootComponent->Draw(graphics);
	}
}

void Actor::RenderShadowMap(Graphics& graphics)
{
	if (rootComponent)
	{
		rootComponent->RenderShadowMap(graphics);
	}
}

void Actor::CreateDefaultRoot()
{
	rootComponent = SceneComponent::CreateComponent();
}

std::string Actor::GetActorFullName()
{
	std::stringstream ss;
	ss << actorName << " " << typeid(*this).name() << "##" << this;
	return ss.str();
}

void Actor::LoadFromFile(Graphics& graphics, const std::string& fileName)
{
	Assimp::Importer importer;

	const aiScene* const scene = importer.ReadFile(fileName,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_GenNormals
	);

	if (!scene)
	{
		throw std::runtime_error(importer.GetErrorString());
	}

	if (scene->mRootNode)
	{
		std::unique_ptr<SceneComponent> newRootComponent;
		if (scene->mRootNode->mNumMeshes == 0)
		{
			newRootComponent = std::move(SceneComponent::CreateComponent(graphics, scene->mRootNode, scene));
		}
		else
		{
			newRootComponent = std::move(MeshComponent::CreateComponent(graphics, scene->mRootNode, scene));
		}
		SetRootComponent<SceneComponent>(std::move(newRootComponent));
	}
}