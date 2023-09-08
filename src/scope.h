#ifndef ECS_SCOPE_H
#define ECS_SCOPE_H

#include "pool.h"

class Scope	final
{
public:

	class Iterator
	{
	public:

		Iterator(Scene* scene, std::vector<Pool*> pools, EntityIndex firstIndex);

		Entity operator*();

		bool operator==(const Iterator& other);
		bool operator!=(const Iterator& other);

		Iterator& operator++();

	private:

		EntityIndex mIndex;
		Scene* mScene;
		std::vector<Pool*> mPools;
	
	private:
	
		bool isIndexValid();

	};

	const Iterator begin();
	const Iterator end();

	~Scope() = default;
	Scope& operator=(const Scope&) = default;
	Scope(const Scope&) = default;


protected:

	Scope(Scene* scene, std::vector<Pool*> pools, bool all);

	friend class Scene;

private:

	Scene* mScene;
	std::vector<Pool*> mPools;
	bool mAll;

};


#endif // !ECS_SCOPE_H
