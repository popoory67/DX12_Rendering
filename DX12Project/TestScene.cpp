#include "stdafx.h"
#include "TestScene.h"
#include "Mesh.h"
#include "Material.h"
#include "Entity.h"
#include "EntityQuery.h"

//void TransformSystem::Update(std::shared_ptr<EntityQuery> InQuery)
//{
//	for (auto& it : Objects)
//	{
//		Object* pObject = it.second;
//		// Only update the cbuffer data if the constants have changed.  
//		// This needs to be tracked per frame resource.
//		if (pObject->UpdateFrameRate > 0)
//		{
//			XMMATRIX world = XMLoadFloat4x4(&pObject->GetWorld());
//			XMMATRIX texTransform = XMLoadFloat4x4(&pObject->GetTextureTransform());
//
//			Transform objConstants;
//			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
//			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
//
//			ObjectBuffer.Get()->CopyData(pObject->ObjCBIndex, objConstants); // Draw에서 쓰려고
//
//			// Next FrameResource need to be updated too.
//			pObject->UpdateFrameRate--;
//		}
//	}
//}

///////////////////////////////////////////////////////////////////////////////////

TestScene::TestScene()
{
//	TestTransform.reset(new TransformSystem());
}

TestScene::~TestScene()
{
}

void TestScene::Start()
{
	BuildRenderItems();

	Entity* pEntity = new Entity();
	if (pEntity)
	{
		//	pEntity->ObjCBIndex = 0;

		pEntity->AddComponent(MaterialData);
		pEntity->AddComponent(PrimitiveData);

		AddEntity(pEntity);
	}
}

void TestScene::Update()
{
//	TestTransform->Update(EntityInterface);
}

void TestScene::End()
{
}

void TestScene::BuildRenderItems()
{
	// Material
	MaterialData.reset(new MaterialComponent());

	// Material
	//woodCrate->Name = "woodCrate";
	//woodCrate->MatCBIndex = 0;
	//woodCrate->DiffuseSrvHeapIndex = 0;

	MaterialData->AddShaderBatch<XMFLOAT4>("DiffuseAlbedo", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	MaterialData->AddShaderBatch<XMFLOAT3>("FresnelR0", XMFLOAT3(0.05f, 0.05f, 0.05f));
	MaterialData->AddShaderBatch<float>("Roughness", 0.2f);
	MaterialData->AddShaderBatch<XMFLOAT4X4>("MatTransform", MathHelper::Identity4x4());

	// Primitive
	PrimitiveData.reset(new PrimitiveComponent());

	PrimitiveData->AddShaderBatch<XMFLOAT4X4>("World", MathHelper::Identity4x4());
	PrimitiveData->AddShaderBatch<XMFLOAT4X4>("TexTransform", MathHelper::Identity4x4());

	PrimitiveData->CreateMesh("");
}
