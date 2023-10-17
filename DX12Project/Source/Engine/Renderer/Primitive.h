#pragma once

// DX
#include <DirectXCollision.h>
struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TextureCoordinate;

	//Vertex() = default;
	//Vertex(const Vertex&) = default;
	//Vertex& operator=(const Vertex&) = default;

	//Vertex(Vertex&&) = default;
	//Vertex& operator=(Vertex&&) = default;
};