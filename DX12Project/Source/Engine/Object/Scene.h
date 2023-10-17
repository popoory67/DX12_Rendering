#pragma once
#include "Entity.h"
#include "Util.h"
#include <bitset>
#include <memory>
#include <unordered_map>

class Scene : public std::enable_shared_from_this<Scene>
{
public:
	Scene();
	virtual ~Scene();

	virtual void Start();
	virtual void Update();
	virtual void End();

	int GetSceneId() { return Id; }
	void SetSceneId(int InId) { Id = InId; }

	void AddEntity(std::shared_ptr<class Entity> InEntity);
	void AddPrimitive(class PrimitiveComponent* InPrimitiveComponent);

	int AddNewCamera();
	void SetMainCamera(int InCameraId);
	const class Camera* GetMainCamera();
	class Camera* GetCamera(int InCameraId);

private:
	void UpdateVisibility();
	bool IsVisible(int InId);

	void UpdateComponents();
	void UpdateCamera(const class RHICommandContext& InContext);

	void RenderScene();

private:
	int Id = INVALID_INDEX;

	std::unordered_map<size_t, std::bitset<1>> EntityVisibility;
	std::unordered_map<size_t, std::shared_ptr<class Entity>> Entities;
	std::vector<std::shared_ptr<Component>> Components;

	std::unordered_map<class PrimitiveProxy*, std::bitset<1>> Primitives;

	std::vector<class Camera*> Cameras;
	class Camera* MainCamera = nullptr;
};