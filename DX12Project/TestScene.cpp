#include "TestScene.h"
#include "TestEntities.h"

TestScene::TestScene()
{

}

TestScene::~TestScene()
{

}

void TestScene::Start()
{
    Wolf = std::make_shared<ModelEntity>(this);
    {
        AddEntity(Wolf);
        
        // TODO
        // A dependency between a transformation and vertex processing must be addressed regardless of the order.
        Wolf->SetRotation(0.0f, 45.0f, 0.0f);

        if (auto mesh = Wolf->GetComponent<MeshComponent>())
        {
            mesh->SetMeshModel(L"../Resources/Obj/wolf/Wolf_One_obj.obj");
        }

        if (auto material = Wolf->GetComponent<MaterialComponent>())
        {
            material->SetTexture(L"../Resources/Obj/wolf/textures/Wolf_Body.jpg");
            material->SetShader(L"./Shaders/ByteCodes/TestShader_vs.cso", ShaderType::Vertex);
            material->SetShader(L"./Shaders/ByteCodes/TestShader_fs.cso", ShaderType::Fragment);
        }
    }

    // TODO
    // It's not rendered more than one model.
    //Diamond = std::make_shared<ModelEntity>(this);
    //{
    //    AddEntity(Diamond);
    //    Diamond->SetPosition(0.0f, 0.0f, -5.0f);
    //    Diamond->SetScale(10.0f, 10.0f, 10.0f);
    //    Diamond->SetRotation(45.0f, 0.0f, 0.0f);
    //    Diamond->SetStaticMesh(/*L"../Resources/Obj/diamond.obj"*/L"../Resources/Obj/wolf/Wolf_One_obj.obj");
    //}

    Camera = std::make_shared<CameraEntity>(this);
    {
        AddEntity(Camera);
    }

    Parent::Start();
}

void TestScene::Update()
{
    Parent::Update();
}
