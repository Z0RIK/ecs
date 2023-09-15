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
	SKYBLUE,
	GREEN
};

const int MINIMUM_AMOUNT = 50;
const int MAXIMUM_AMOUNT = 300;

const int MINIMUM_FORCE = -100'000;
const int MAXIMUM_FORCE = 100'000;

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

Scene* createScene(std::vector<size_t> startingAmount, Rectangle simulationBorders)
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
			data->position = { rand() % (int)simulationBorders.width + simulationBorders.x, rand() % (int)simulationBorders.height + simulationBorders.y };
			data->velocity = { 0.0f, 0.0f };
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

	std::vector<size_t> startingAmount(4, 0);
	std::vector<std::vector<float>> forceTable(4, std::vector<float>(4, 0.0f));

	startingAmount[0] = 300;
	startingAmount[1] = 100;
	startingAmount[2] = 100;
	startingAmount[3] = 150;

	InitWindow(900, 600, "[core]");
	SetTargetFPS(60);

	Rectangle uiBorders(10, 10, 280, 580);
	Rectangle simulationBorders(300, 10, 590, 580);

	Scene* scene = createScene(startingAmount, simulationBorders);

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

			v1->x = 0.0f;
			v1->y = 0.0f;

			for (Entity entity2 : scene->scope<ParticleData>())
			{
				ParticleData* data2 = scene->getComponent<ParticleData>(entity2);
				Vector2* pos2 = &data2->position;
				size_t id2 = data2->id;

				if (forceTable[id1][id2] == 0.0f) continue;

				Vector2 distanceVector = { pos2->x - pos1->x, pos2->y - pos1->y };
				float distance = sqrt(distanceVector.x * distanceVector.x + distanceVector.y * distanceVector.y);

				if (distance <= 1.0f) distance = 0.1f;

				v1->x += (distanceVector.x / (distance * distance * distance)) * forceTable[id1][id2] * GetFrameTime();
				v1->y += (distanceVector.y / (distance * distance * distance)) * forceTable[id1][id2] * GetFrameTime();
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

			if (pos->x < simulationBorders.x || pos->x >(simulationBorders.x + simulationBorders.width))
			{
				v->x *= -1.0f;
				pos->x += 2 * v->x * GetFrameTime();
			}
			if (pos->y < simulationBorders.y || pos->y >(simulationBorders.y + simulationBorders.height))
			{
				v->y *= -1.0f;
				pos->y += 2 * v->y * GetFrameTime();
			}

			// Age
			data->age -= GetFrameTime();

			//if (data->age <= 0.0f) 
			//	scene->destroyEntity(entity);
			
		}

		// Draw
		BeginDrawing();

		ClearBackground(DARKGRAY);
		
		for (Entity entity : scene->scope<ParticleData, Sprite>())
		{
			ParticleData* data = scene->getComponent<ParticleData>(entity);
			Vector2* pos = &data->position;

			Sprite* spr = scene->getComponent<Sprite>(entity);

			DrawCircle(pos->x, pos->y, spr->radius, spr->color);
		}

		DrawText(TextFormat("Entity count : [%d]", scene->size()), 20, 20, 10, RAYWHITE);

		float yPos = 50.0f;

		for (size_t i = 0; i < 4; i++)
		{
			DrawText("Particle ", 20, yPos, 10, RAYWHITE);
			DrawRectangle(20 + GetTextWidth("Particle "), yPos, 10, 10, colors[i]);
			yPos += 20.0f;
			DrawText("Starting amount ", 20, yPos, 10, RAYWHITE);
			GuiValueBox(Rectangle(20 + GetTextWidth("Starting amount "), yPos, 50, 10), nullptr, (int*)&startingAmount[i], MINIMUM_AMOUNT, MAXIMUM_AMOUNT, false);
			for (size_t j = 0; j < 4; j++)
			{
				yPos += 20.0f;
				DrawRectangle(20, yPos, 10, 10, colors[i]);
				DrawLineEx({ 35, yPos + 10 }, { 45, yPos }, 1, RAYWHITE);
				DrawLineEx({ 35, yPos }, { 45, yPos + 10 }, 1, RAYWHITE);
				DrawRectangle(50, yPos, 10, 10, colors[j]);
				GuiSlider(Rectangle(70, yPos, 140, 10), nullptr, TextFormat("%.3f", forceTable[i][j]), &forceTable[i][j], MINIMUM_FORCE, MAXIMUM_FORCE);
			}
			yPos += 20.0f;
		}

		if (GuiButton(Rectangle(200, yPos, GetTextWidth("Reset scene") + 10, 20), "Reset scene"))
		{
			delete scene;
			scene = createScene(startingAmount, simulationBorders);
		}

		if (GuiButton(Rectangle(100, yPos, GetTextWidth("Random weights") + 10, 20), "Random"))
		{
			for (size_t i = 0; i < 4; i++)
			{
				startingAmount[i] = rand() % (MAXIMUM_AMOUNT - MINIMUM_AMOUNT) + MINIMUM_AMOUNT;
				for (size_t j = 0; j < 4; j++)
					forceTable[i][j] = static_cast<float>(10 * (rand() % (10'000 + 10'000) - 10'000));
			}
		}

		DrawRectangleLinesEx(uiBorders, 2, RAYWHITE);
		DrawRectangleLinesEx(simulationBorders, 2, RAYWHITE);

		EndDrawing();
	}

	return 0;
}