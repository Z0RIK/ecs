#ifndef ECS_H
#define ECS_H

#include <vector>

#define MAX_ENTITIES UINT32_MAX - 1 // UINT32_MAX is reserved for invalid indexes

typedef uint64_t Entity;
typedef uint32_t EntityIndex;
typedef uint32_t EntityVersion;

#include "pool.h"
#include "scope.h"
#include "scene.h"

#endif // !ECS_H