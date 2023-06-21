#pragma once

// The reason why it is not a class but a struct is
// that we don't need to make sure ResourceType is a public type.
template<class T>
struct TD3D12Types
{
	// using ResourceType = D3D12Type;
};