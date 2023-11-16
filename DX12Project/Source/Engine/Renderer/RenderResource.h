#pragma once
#include <intsafe.h>

enum class ShaderType : unsigned int
{
	Vertex,
	Fragment,
};

struct ShaderBinding
{
	SIZE_T BytecodeLength;
	BYTE* Bytecode = nullptr;
	ShaderType Type;
};

struct TextureSettings
{
	BYTE* TextureData = nullptr;
	size_t Width = 0;
	size_t Height = 0;
	unsigned short Channels = 3;
	unsigned short MipLevel = 1;
	unsigned short Format = 26; // DXGI_FORMAT_R11G11B10_FLOAT
};