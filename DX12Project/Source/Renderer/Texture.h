#pragma once

using namespace Microsoft::WRL;

struct TextureData
{
	// Unique material name for lookup.
	std::string Name;
	std::wstring Filename;
};