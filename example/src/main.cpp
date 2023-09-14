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
			*spr = { colors[i], 1.0f};
		}
	}

	return scene;
}

int main()
{
	float lifeTime{};
	float spawnRate{}, spawnCounter{};

	std::vector<size_t> startingAmount(4, 150);
	std::vector<std::vector<float>> forceTable(4, std::vector<float>(4, 0.0f));

	forceTable[0][0] = -2000.0f;
	forceTable[0][1] = 6000.0f;
	forceTable[1][0] = 800.0f;
	forceTable[1][1] = 6000.0f;

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

		DrawRectangleLinesEx(uiBorders, 2, LIGHTGRAY);
		DrawRectangleLinesEx(simulationBorders, 2, LIGHTGRAY);

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

				if (distance <= 0.01f) continue;

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

			if (pos->x < simulationBorders.x || pos->x > (simulationBorders.x + simulationBorders.width))
				v->x *= -1.0f;
			if (pos->y < simulationBorders.y || pos->y > (simulationBorders.y + simulationBorders.height)) 
				v->y *= -1.0f;

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

		DrawText(TextFormat("Entity count : [%d]", scene->size()), 10, 10, 10, LIGHTGRAY);

		float yPos = 50.0f;

		for (size_t i = 0; i < 4; i++)
		{
			DrawText("Particle ", 15, yPos, 10, LIGHTGRAY);
			DrawRectangle(15 + GetTextWidth("Particle "), yPos, 10, 10, colors[i]);
			yPos += 20.0f;
			DrawText("Starting amount ", 15, yPos, 10, LIGHTGRAY);
			GuiValueBox(Rectangle(15 + GetTextWidth("Starting amount "), yPos, 50, 10), nullptr, (int*)&startingAmount[i], 0, 10'000, false);
			for (size_t j = 0; j < 4; j++)
			{
				yPos += 20.0f;
				DrawRectangle(15, yPos, 10, 10, colors[i]);
				DrawLineEx({ 30, yPos + 10 }, { 40, yPos }, 1, LIGHTGRAY);
				DrawLineEx({ 30, yPos }, { 40, yPos + 10 }, 1, LIGHTGRAY);
				DrawRectangle(45, yPos, 10, 10, colors[j]);
				GuiSlider(Rectangle(65, yPos, 140, 10), nullptr, std::to_string(forceTable[i][j]).c_str(), &forceTable[i][j], -10'000, 10'000);
			}
			yPos += 20.0f;
		}

		if (GuiButton(Rectangle(200, yPos, GetTextWidth("Reset scene") + 10, 20), "Reset scene"))
		{
			delete scene;
			scene = createScene(startingAmount, simulationBorders);
		}

		//GuiSlider(Rectangle(10, 50, 200, 20), nullptr, std::to_string(forceTable[0][0]).c_str(), &forceTable[0][0], -10'000, 10'000);
		//DrawRectangle(10, 70, 10, 10, colors[0]);
		//DrawRectangle(10, 90, 10, 10, colors[1]);
		//DrawRectangle(10, 110, 10, 10, colors[2]);
		//DrawRectangle(10, 130, 10, 10, colors[3]);

		EndDrawing();
	}

	return 0;
}