#include <random>
#include <iostream>

#include "ecs/ecs.h"

#include "raylib.h"

typedef struct EntityData {
	Vector2 position;
	Vector2 velocity;
	float mass;
	float age;
} PhysicsData;

typedef struct Sprite {
	Color color;
	float radius;
} Sprite;

Scene* createScene()
{
	Scene* scene = new Scene(1024);

	scene->createPool<EntityData>(1024);
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
			EntityData* data = scene->assignComponent<EntityData>(newEntity);

			data->position = { float(rand() % GetScreenWidth()), float(rand() % GetScreenHeight()) };
			data->velocity = { float(rand() % 256 - 128), float(rand() % 256 - 128) };
			data->mass = rand() % 255;
			data->age = rand() % 255;

			Sprite* spr = scene->assignComponent<Sprite>(newEntity);
			*spr = { DARKGRAY, 10.0f };
		}

		// Update position

		for (Entity entity : scene->scope<EntityData>())
		{
			EntityData* data = scene->getComponent<EntityData>(entity);
			Vector2* pos = &data->position;
			Vector2* v = &data->velocity;

			pos->x += v->x * GetFrameTime(); 
			pos->y += v->y * GetFrameTime();

			if (pos->x < 0.0f || pos->x > GetScreenWidth())
				v->x *= -1.0f;
			if (pos->y < 0.0f || pos->y > GetScreenHeight()) 
				v->y *= -1.0f;
		}

		// Draw
		BeginDrawing();

		ClearBackground(LIGHTGRAY);
		
		for (Entity entity : scene->scope<EntityData, Sprite>())
		{
			EntityData* data = scene->getComponent<EntityData>(entity);
			Vector2* pos = &data->position;

			Sprite* spr = scene->getComponent<Sprite>(entity);

			DrawCircle(pos->x, pos->y, spr->radius, spr->color);
		}

		DrawText(TextFormat("Entity count : [%d]", scene->size()), 10, 10, 10, MAGENTA);

		EndDrawing();
	}

	return 0;
}