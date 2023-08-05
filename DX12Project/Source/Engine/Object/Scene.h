#pragma once
#include <bitset>
#include <memory>
#include <unordered_map>

class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual void Start();
	virtual void Update();
	virtual void End();

	unsigned GetSceneId() { return Id; }
	void SetSceneId(unsigned InId) { Id = InId; }

	void AddEntity(std::shared_ptr<class Entity> InEntity);
	void AddPrimitive(class PrimitiveComponent* InPrimitiveComponent);

private:
	void UpdateVisibility();
	bool IsVisible(unsigned InId);

	void RenderScene();

private:
	unsigned int Id = 0;

	std::unordered_map<size_t, std::bitset<1>> EntityVisibility;
	std::unordered_map<size_t, std::shared_ptr<class Entity>> Entities;

	std::unordered_map<class PrimitiveProxy*, std::bitset<1>> Primitives;
};