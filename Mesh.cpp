#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>
#include <d3dcompiler.h>
#include "ThrowMacros.h"
#include <string>
#include "Graphics.h"
#include <cassert>
#include <d3d11.h>

Mesh::Mesh(Graphics& graphics, std::string fileName, ShaderType shaderType, DirectX::XMVECTOR position, DirectX::XMVECTOR rotation, DirectX::XMVECTOR scale) :
	position(position),
	rotation(rotation),
	scale(scale)
{
	LoadModel(fileName);
	assert(vertices.size() > 0 && indices.size() > 0);

	std::wstring pixelShaderPath;
	std::wstring vertexShaderPath;

	Microsoft::WRL::ComPtr<ID3DBlob> blob;

	switch (shaderType)
	{
	case Solid:
		pixelShaderPath = L"PixelShader.cso";
		vertexShaderPath = L"VertexShader.cso";
		break;
	case Phong:
		pixelShaderPath = L"PhongPS.cso";
		vertexShaderPath = L"PhongVS.cso";
		break;
	}

	D3D11_BUFFER_DESC colorBufferDesc{};
	colorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	colorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	colorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	colorBufferDesc.ByteWidth = sizeof(colorBuffer);
	D3D11_SUBRESOURCE_DATA colorBufferData = {};
	colorBufferData.pSysMem = &colorBuffer;

	CHECK_HR(graphics.device->CreateBuffer(&colorBufferDesc, &colorBufferData, &constantColorBuffer));

	CHECK_HR(D3DReadFileToBlob(pixelShaderPath.c_str(), &blob));
	CHECK_HR(graphics.device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader));

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.CPUAccessFlags = 0u;
	indexBufferDesc.MiscFlags = 0u;
	indexBufferDesc.ByteWidth = (UINT)indices.size() * sizeof(unsigned int);
	indexBufferDesc.StructureByteStride = sizeof(unsigned int);

	D3D11_SUBRESOURCE_DATA indexBufferData = {};
	indexBufferData.pSysMem = indices.data();

	CHECK_HR(graphics.device->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer));

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.CPUAccessFlags = 0u;
	vertexBufferDesc.MiscFlags = 0u;
	vertexBufferDesc.ByteWidth = (UINT)vertices.size() * sizeof(Vertex);
	vertexBufferDesc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA vertexBufferData = {};
	vertexBufferData.pSysMem = vertices.data();

	CHECK_HR(graphics.device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer));

	D3D11_BUFFER_DESC constantBufferDesc{};
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0u;
	constantBufferDesc.ByteWidth = sizeof(transformBuffer);
	constantBufferDesc.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA constanBufferData = {};

	constanBufferData.pSysMem = &transformBuffer;
	CHECK_HR(graphics.device->CreateBuffer(&constantBufferDesc, &constanBufferData, &constantTransformBuffer));

	CHECK_HR(D3DReadFileToBlob(vertexShaderPath.c_str(), &blob));
	CHECK_HR(graphics.device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader));

	D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
	{
		{"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u},
		{"NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u},
	};
	CHECK_HR(graphics.device->CreateInputLayout(inputLayoutDesc, (UINT)std::size(inputLayoutDesc), blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout));
}

void Mesh::Draw(Graphics& graphics)
{
	graphics.context->PSSetShader(pixelShader.Get(), nullptr, 0u);
	graphics.context->PSSetConstantBuffers(0u, 1u, constantColorBuffer.GetAddressOf());

	graphics.context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	graphics.context->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);

	UpdateTransformBuffer(graphics);
	graphics.context->VSSetConstantBuffers(0u, 1u, constantTransformBuffer.GetAddressOf());

	graphics.context->VSSetShader(vertexShader.Get(), nullptr, 0u);

	graphics.context->IASetInputLayout(inputLayout.Get());

	graphics.DrawIndexed(indices.size());
}

void Mesh::AddRotation(const DirectX::XMVECTOR& rotationToAdd) noexcept
{
	rotation = DirectX::XMVectorAdd(rotation, rotationToAdd);
}

void Mesh::AddPosition(const DirectX::XMVECTOR& posiationToAdd) noexcept
{
	position = DirectX::XMVectorAdd(position, posiationToAdd);
}

void Mesh::Scale(float scaleFactor) noexcept
{
	scale = DirectX::XMVectorScale(scale, scaleFactor);
}

void Mesh::SetColor(Graphics& graphics, const DirectX::XMFLOAT4& newColor)
{
	colorBuffer = newColor;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	CHECK_HR(graphics.context->Map(constantColorBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedSubresource));
	memcpy(mappedSubresource.pData, &colorBuffer, sizeof(colorBuffer));
	graphics.context->Unmap(constantColorBuffer.Get(), 0u);
}

DirectX::XMMATRIX Mesh::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixScalingFromVector(scale) * DirectX::XMMatrixRotationRollPitchYawFromVector(rotation) * DirectX::XMMatrixTranslationFromVector(position);
}

void Mesh::LoadModel(std::string fileName)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(fileName,
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

	const unsigned int numVertices = scene->mMeshes[0]->mNumVertices;

	vertices.reserve(numVertices);

	assert(scene->mMeshes[0]->HasNormals());
	for (size_t i = 0; i < numVertices; i++)
	{
		vertices.push_back(Vertex(scene->mMeshes[0]->mVertices[i].x, scene->mMeshes[0]->mVertices[i].y, scene->mMeshes[0]->mVertices[i].z,
			scene->mMeshes[0]->mNormals[i].x, scene->mMeshes[0]->mNormals[i].y, scene->mMeshes[0]->mNormals[i].z));
	}

	for (size_t i = 0; i < scene->mMeshes[0]->mNumFaces; i++)
	{
		for (size_t j = 0; j < scene->mMeshes[0]->mFaces[i].mNumIndices; j++)
		{
			indices.push_back(scene->mMeshes[0]->mFaces[i].mIndices[j]);
		}
	}
}

void Mesh::UpdateTransformBuffer(Graphics& graphics)
{
	DirectX::XMMATRIX transformView = DirectX::XMMatrixTranspose(GetTransformMatrix() * graphics.GetCamera());
	DirectX::XMMATRIX transformViewProjection = DirectX::XMMatrixTranspose(GetTransformMatrix() * graphics.GetCamera() * graphics.GetProjection());
	transformBuffer = ConstantTransformBuffer(std::move(transformView), std::move(transformViewProjection));
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	CHECK_HR(graphics.context->Map(constantTransformBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedSubresource));
	memcpy(mappedSubresource.pData, &transformBuffer, sizeof(transformBuffer));
	graphics.context->Unmap(constantTransformBuffer.Get(), 0u);
}