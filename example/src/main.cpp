#include <random>
#include <iostream>
#include <vector>

#include "ecs/ecs.h"

#include "raylib.h"

typedef struct ParticleData {
	Vector2 position;
	Vector2 velocity;
	float age;
} ParticleData;

typedef struct Sprite {
	Color color;
	float radius;
} Sprite;

Scene* createScene()
{
	Scene* scene = new Scene(1024);

	scene->createPool<ParticleData>(1024);
	scene->createPool<Sprite>(1024);

	return scene;
}

int main()
{
	float lifeTime{};
	float spawnRate{}, spawnCounter{};

	Scene* scene = createScene();

	InitWindow(800, 600, "[core]");
	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
		// Update

		// Create new entity 
		spawnRate = 1.0f;

		spawnCounter -= GetFrameTime();
		if (spawnCounter <= 0.0f)
		{
			spawnCounter = spawnRate;

			Entity newEntity = scene->createEntity();
			ParticleData* data = scene->assignComponent<ParticleData>(newEntity);

			data->position = { float(rand() % GetScreenWidth()), float(rand() % GetScreenHeight()) };
			data->velocity = { float(rand() % 256 - 128), float(rand() % 256 - 128) };
			data->age = rand() % 255;

			Sprite* spr = scene->assignComponent<Sprite>(newEntity);
			*spr = { DARKGRAY, 10.0f };
		}

		// Update velocity

		for (Entity entity1 : scene->scope<ParticleData>())
		{
			ParticleData* data1 = scene->assignComponent<ParticleData>(entity1);
			Vector2* pos1 = &data1->position;

			for (Entity entity2 : scene->scope<ParticleData>())
			{
				ParticleData* data2 = scene->assignComponent<ParticleData>(entity2);
				Vector2* pos2 = &data2->position;

				Vector2 distanceVector = { pos2->x - pos1->x, pos2->y - pos1->x };
				float distance = sqrt(distanceVector.x * distanceVector.x + distanceVector.y * distanceVector.y);

			}
		}

		// Update position and age

		for (Entity entity : scene->scope<ParticleData>())
		{
			// Position
			ParticleData* data = scene->getComponent<ParticleData>(entity);
			Vector2* pos = &data->position;
			Vector2* v = &data->velocity;

			pos->x += v->x * GetFrameTime(); 
			pos->y += v->y * GetFrameTime();

			if (pos->x < 0.0f || pos->x > GetScreenWidth())
				v->x *= -1.0f;
			if (pos->y < 0.0f || pos->y > GetScreenHeight()) 
				v->y *= -1.0f;

			// Age
			data->age -= GetFrameTime();

			if (data->age <= 0.0f) 
				scene->destroyEntity(entity);
			
		}

		// Draw
		BeginDrawing();

		ClearBackground(LIGHTGRAY);
		
		for (Entity entity : scene->scope<ParticleData, Sprite>())
		{
			ParticleData* data = scene->getComponent<ParticleData>(entity);
			Vector2* pos = &data->position;

			Sprite* spr = scene->getComponent<Sprite>(entity);

			DrawCircle(pos->x, pos->y, spr->radius, spr->color);
		}

		DrawText(TextFormat("Entity count : [%d]", scene->size()), 10, 10, 10, MAGENTA);

		EndDrawing();
	}

	return 0;
}