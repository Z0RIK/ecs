#ifndef ECS_SCENE_H
#define ECS_SCENE_H

#include <vector>

typedef uint64_t Entity;
typedef uint32_t EntityIndex;
typedef uint32_t EntityVersion;

#include "pool.h"
#include "scope.h"

static size_t sComponentCounter = 0;

template<typename T>
int getComponentId()
{
	static int sComponentId = sComponentCounter++;
	return sComponentId;
}

class Scene final
{
public:

	Scene(size_t capacity);
	~Scene();

	Entity createEntity();
	void destroyEntity(Entity entity);
	inline bool isValid(Entity entity);
	
	template <typename T>
	T* assignComponent(Entity entity);

	template <typename T>
	void removeComponent(Entity entity);

	template <typename T>
	inline T* getComponent(Entity entity);

	template <typename... ComponentTypes>
	Scope scope();

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

protected:

	std::vector<EntityIndex> mFreeIndexes;
	std::vector<Entity> mEntities;
	std::vector<Pool*> mPools;

	friend class Scope;

private:

	inline Entity combineIndexVersion(EntityIndex index, EntityVersion version);
	inline EntityIndex getEntityIndex(Entity entity);
	inline EntityVersion getEntityVersion(Entity entity);
	inline bool isEntityValid(Entity entity);

};

template<typename T>
T* Scene::assignComponent(Entity entity)
{
	if (mEntities[getEntityIndex(entity)] != entity)
		return nullptr;

	size_t componentId = getComponentId<T>();

	while (componentId >= mPools.size())
	{
		mPools.push_back(nullptr);
		mPools.back() = new Pool(sizeof(T), mEntities.capacity());
	}

	T* component = nullptr;
	component = new (mPools[componentId]->assign(getEntityIndex(entity))) T();

	return component;
}

template<typename T>
void Scene::removeComponent(Entity entity)
{
	if (mEntities[getEntityIndex(entity)] != entity)
		return;

	size_t componentId = getComponentId<T>();
	mPools[componentId]->remove(getEntityIndex(entity));
	return;
}

template<typename T>
inline T* Scene::getComponent(Entity entity)
{
	if (mEntities[getEntityIndex(entity)] != entity)
		return nullptr;

	size_t componentId = getComponentId<T>();

	if (componentId >= mPools.size())
		return nullptr;

	return (T*)mPools[componentId]->get(getEntityIndex(entity));
}

template <typename... ComponentTypes>
Scope Scene::scope()
{
	if (!sizeof...(ComponentTypes)) return Scope(this, mPools, true);

	int componentIds[sizeof...(ComponentTypes)] = { getComponentId<ComponentTypes...>() };

	std::vector<Pool*> pools;

	for (size_t i = 0; i < sizeof...(ComponentTypes); i++)
		pools.push_back(mPools[componentIds[i]]);

	return Scope(this, pools, false);
}

#endif // !ECS_SCENE_H


