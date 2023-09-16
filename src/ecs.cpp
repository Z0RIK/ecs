#include "ecs/ecs.h"

//__SCENE______________________________________________________________________

Scene::Scene(size_t capacity)
{
	mEntities.reserve(capacity);
}

Scene::~Scene()
{
	for (Pool* pool : mPools)
		delete pool;
}

Entity Scene::createEntity()
{
	if (!mFreeIndexes.empty())
	{
		EntityIndex newIndex = mFreeIndexes.back();
		EntityVersion newVersion = getEntityVersion(mEntities[newIndex]);
		mEntities[newIndex] = combineIndexVersion(newIndex, newVersion);

		mFreeIndexes.pop_back();

		return mEntities[newIndex];
	}

	mEntities.push_back(combineIndexVersion((EntityIndex)mEntities.size(), EntityVersion(0)));

	return mEntities.back();
}

void Scene::destroyEntity(Entity entity)
{
	for (Pool* pool : mPools)
		pool->remove(getEntityIndex(entity));

	mFreeIndexes.push_back(getEntityIndex(entity));

	EntityIndex newIndex = EntityIndex(-1);
	EntityVersion newVersion = getEntityVersion(entity) + 1;

	mEntities[getEntityIndex(entity)] = combineIndexVersion(newIndex, newVersion);
}

bool Scene::isValid(Entity entity)
{
	return (getEntityIndex(entity) != EntityIndex(-1)) 
		&& (getEntityIndex(entity) < mEntities.size())
		&& (mEntities[getEntityIndex(entity)] == entity);
}

size_t Scene::size()
{
	return mEntities.size() - mFreeIndexes.size();
}

Entity Scene::combineIndexVersion(EntityIndex index, EntityVersion version)
{
	return ((Entity)index << 32) | ((Entity)version);
}

EntityIndex Scene::getEntityIndex(Entity entity)
{
	return entity >> 32;
}

EntityVersion Scene::getEntityVersion(Entity entity)
{
	return (EntityVersion)entity;
}

bool Scene::isEntityValid(Entity entity)
{
	return entity >> 32 != EntityIndex(-1);
}

//_!SCENE______________________________________________________________________
//_____________________________________________________________________________

//__POOL_______________________________________________________________________

Pool::Pool(size_t elementSize, size_t poolSize)
	:mElementSize(elementSize), mChunkSize(poolSize)
{
	mData.push_back(std::vector<uint8_t>());
	mData.back().reserve(mChunkSize * mElementSize);
}

void* Pool::assign(EntityIndex entityIndex)
{
	if (mPointers.size() <= entityIndex)
		mPointers.resize(entityIndex + 1, nullptr);

	if (mPointers[entityIndex]) 
		return mPointers[entityIndex];

	if (!mFreePointers.empty())
	{
		mPointers[entityIndex] = mFreePointers.back();
		mFreePointers.pop_back();

		return mPointers[entityIndex];
	}

	if (mData.back().size() == mChunkSize * mElementSize)
	{
		mData.push_back(std::vector<uint8_t>());
		mData.back().reserve(mChunkSize * mElementSize);
	}

	mData.back().resize(mData.back().size() + mElementSize);

 	mPointers[entityIndex] = &mData.back()[0] + mData.back().size() - mElementSize;

	return mPointers[entityIndex];
}

void Pool::remove(EntityIndex entityIndex)
{
	if (mPointers.size() <= entityIndex) return;

	if (mPointers[entityIndex])
		mFreePointers.push_back(mPointers[entityIndex]);

	mPointers[entityIndex] = nullptr;
}

void* Pool::get(EntityIndex entityIndex)
{
	return (mPointers.size() <= entityIndex) ? nullptr : mPointers[entityIndex];
}

//_!POOL_______________________________________________________________________
//_____________________________________________________________________________

//__SCOPE______________________________________________________________________

Scope::Scope(Scene* scene, std::vector<Pool*> pools, bool all)
	:mScene(scene), mPools(pools), mAll(all)
{}

const Scope::Iterator Scope::begin()
{
	if (mAll && mScene->mPools.size() != mPools.size())
		mPools = mScene->mPools;

	return ++Iterator(mScene, mPools, EntityIndex(-1));
}

const Scope::Iterator Scope::end()
{
	if (mAll && mScene->mPools.size() != mPools.size())
		mPools = mScene->mPools;

	return Iterator(mScene, mPools, (EntityIndex)mScene->mEntities.size());
}

Scope::Iterator::Iterator(Scene* scene, std::vector<Pool*> pools, EntityIndex firstIndex)
	:mScene(scene), mPools(pools), mIndex(firstIndex)
{}

Entity Scope::Iterator::operator*()
{
	return mScene->mEntities[mIndex];
}

bool Scope::Iterator::operator==(const Iterator& other)
{
	return mScene == other.mScene && mIndex == other.mIndex;
}

bool Scope::Iterator::operator!=(const Iterator& other)
{
	return mScene != other.mScene || mIndex != other.mIndex;
}

Scope::Iterator& Scope::Iterator::operator++()
{
	do
	{
		mIndex++;
	} while (mIndex < mScene->mEntities.size() && !isIndexValid());

	return *this;
}

bool Scope::Iterator::isIndexValid()
{
	for (auto pool : mPools)
		if (!pool->get(mIndex)) return false;

	return true;
}

//_!SCOPE______________________________________________________________________
//_____________________________________________________________________________