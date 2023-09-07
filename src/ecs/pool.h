#ifndef ECS_POOL_H
#define ECS_POOL_H

class Pool
{
public:

	Pool(const Pool&) = delete;
	Pool& operator=(const Pool&) = delete;

	inline void* get(EntityIndex);

protected:

	Pool(size_t elementSize, size_t chunkSize);
	~Pool() = default;

	void* assign(EntityIndex);
	void remove(EntityIndex);

	friend class Scene;

private:

	size_t mElementSize, mChunkSize;
	std::vector<void*> mPointers;
	std::vector<void*> mFreePointers;
	std::vector<std::vector<uint8_t>> mData;

};

#endif // !ECS_POOL_H