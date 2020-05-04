#pragma once

using namespace Microsoft::WRL;

struct Texture
{
	// Unique material name for lookup.
	std::string Name;
	std::wstring Filename;
};