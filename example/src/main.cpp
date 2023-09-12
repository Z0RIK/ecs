#include <random>
#include <iostream>
#include <vector>
#include <string>

#include "ecs/ecs.h"

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

std::vector<Color> colors =
{
	YELLOW,
	PINK,
	ORANGE,
	RED,
	GOLD,
	MAROON,
	GREEN,
	SKYBLUE,
	LIME,
	DARKGREEN,
	PURPLE,
	MAGENTA,
	VIOLET,
	DARKPURPLE
};

// User defined components
typedef struct ParticleData {
	size_t id;
	Vector2 position;
	Vector2 velocity;
	float age;
} ParticleData;

typedef struct Sprite {
	Color color;
	float radius;
} Sprite;

Scene* createScene(std::vector<size_t> startingAmount)
{
	size_t particleCount{};

	for (auto it : startingAmount)
		particleCount += it;

	Scene* scene = new Scene(particleCount);

	for (size_t i = 0; i < startingAmount.size(); i++)
	{
		for (size_t j = 0; j < startingAmount[i]; j++)
		{
			Entity newEntity = scene->createEntity();
			ParticleData* data = scene->assignComponent<ParticleData>(newEntity);

			data->id = i;
			data->position = { float(rand() % GetScreenWidth()), float(rand() % GetScreenHeight()) };
			data->velocity = { 0.0f, 0.0f };
			//data->velocity = { float(rand() % 50 - 25), float(rand() % 50 - 25) };
			data->age = rand() % 255;

			Sprite* spr = scene->assignComponent<Sprite>(newEntity);
			*spr = { colors[i], 3.0f};
		}
	}

	return scene;
}

int main()
{
	float lifeTime{};
	float spawnRate{}, spawnCounter{};

	std::vector<size_t> startingAmount(2, 300);
	std::vector<std::vector<float>> forceTable(2, std::vector<float>(2, 0.0f));

	forceTable[0][0] = -2000.0f;
	forceTable[0][1] = 6000.0f;
	forceTable[1][0] = 800.0f;
	forceTable[1][1] = 6000.0f;

	InitWindow(800, 600, "[core]");
	SetTargetFPS(60);

	Scene* scene = createScene(startingAmount);

	while (!WindowShouldClose())
	{
		// Update
		/*
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

		*/
		// Update velocity

		for (Entity entity1 : scene->scope<ParticleData>())
		{
			ParticleData* data1 = scene->getComponent<ParticleData>(entity1);
			Vector2* pos1 = &data1->position;
			Vector2* v1 = &data1->velocity;
			size_t id1 = data1->id;

			for (Entity entity2 : scene->scope<ParticleData>())
			{
				ParticleData* data2 = scene->getComponent<ParticleData>(entity2);
				Vector2* pos2 = &data2->position;
				size_t id2 = data2->id;

				if (forceTable[id1][id2] == 0.0f) continue;

				Vector2 distanceVector = { pos2->x - pos1->x, pos2->y - pos1->y };
				float distance = sqrt(distanceVector.x * distanceVector.x + distanceVector.y * distanceVector.y);

				if (distance <= 0.1f) continue;

				v1->x += (distanceVector.x / (distance * distance * distance)) * forceTable[id1][id2] * GetFrameTime();
				v1->y += (distanceVector.y / (distance * distance * distance)) * forceTable[id1][id2] * GetFrameTime();
				//distanceVector.x /= distance;
				//distanceVector.y /= distance;


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

			//if (data->age <= 0.0f) 
			//	scene->destroyEntity(entity);
			
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
		if (GuiButton(Rectangle(30, 30, GetTextWidth("Reset scene") + 10, 20), "Reset scene"))
		{
			delete scene;
			scene = createScene(startingAmount);
		}
		GuiSlider(Rectangle(10, 50, 200, 20), nullptr, std::to_string(forceTable[0][0]).c_str(), &forceTable[0][0], -10'000, 10'000);
		GuiColorPicker(Rectangle(10, 200, 100, 100), nullptr, &colors[0]);

		EndDrawing();
	}

	return 0;
}