#include "stdafx.h"
#include "UpdateManager.h"
#include "Object.h"

UpdateManager::UpdateManager()
{
}


UpdateManager::~UpdateManager()
{
}

void UpdateManager::UpdateTransform(const GameTimer& InTimer)
{
	for (auto& it : Objects)
	{
		Object* pObject = it.second;
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (pObject->UpdateFrameRate > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&pObject->GetWorld());
			XMMATRIX texTransform = XMLoadFloat4x4(&pObject->GetTextureTransform());

			Transform objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));

			ObjectBuffer.Get()->CopyData(pObject->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			pObject->UpdateFrameRate--;
		}
	}
}
