#include "stdafx.h"
#include "TestScene.h"

TestObject::TestObject()
{
	ObjectData = new Object();
}

void TestObject::BuildTestObject()
{
	ObjectData = std::make_unique<Object>();
	boxRitem->ObjCBIndex = 0;
	boxRitem->Mat = mMaterials["woodCrate"].get();
	boxRitem->Geo = mGeometries["boxGeo"].get();
	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	OpaqueRitems.push_back(std::move(boxRitem));

	// 	// All the render items are opaque.
	// 	for (auto& e : AllRitems)
	// 		OpaqueRitems.push_back(e.get());
}

TestScene::TestScene()
{
	Test = new TestObject();
}

TestScene::~TestScene()
{
}