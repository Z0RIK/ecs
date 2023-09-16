#include <iostream>
#include <vector>
#include <string>

#include "ecs/ecs.h"

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// User defined components
struct ParticleData {
	size_t id;
	Vector2 position;
	Vector2 velocity;
	float radius;
	float maxDistance;
};

struct Sprite {
	Color color;
	float radius;
};

// Global variables
std::vector<Color> colors =
{
	YELLOW,
	PINK,
	SKYBLUE,
	GREEN
};

const int MINIMUM_AMOUNT = 50;
const int MAXIMUM_AMOUNT = 300;

const int MINIMUM_FORCE = -100;
const int MAXIMUM_FORCE = 100;

Scene* createScene(std::vector<size_t> startingAmount, Rectangle simulationBorders);

int main()
{
	float lifeTime{};
	float spawnRate{}, spawnCounter{};
	float timeScale = 1.0f;

	std::vector<size_t> startingAmount(4, 100);
	std::vector<bool> startingAmountMode(4, false);
	std::vector<std::vector<float>> forceTable(4, std::vector<float>(4, 0.0f));

	InitWindow(900, 600, "Particle life example");
	SetTargetFPS(60);

	Rectangle uiBorders(10, 10, 280, 580);
	Rectangle simulationBorders(300, 10, 590, 580);

	Scene* scene = createScene(startingAmount, simulationBorders);

	while (!WindowShouldClose())
	{
		// Update
		float deltaTime = GetFrameTime() * timeScale;

		// Update velocity
		for (Entity entity1 : scene->scope<ParticleData>())
		{
			ParticleData* data1 = scene->getComponent<ParticleData>(entity1);
			Vector2* pos1 = &data1->position;
			Vector2* v1 = &data1->velocity;
			size_t id1 = data1->id;
			float radius = data1->radius;
			float maxDistance = data1->maxDistance;
			float halfDistance = (maxDistance - radius) / 2.0f + radius;

			for (Entity entity2 : scene->scope<ParticleData>())
			{
				ParticleData* data2 = scene->getComponent<ParticleData>(entity2);
				Vector2* pos2 = &data2->position;
				size_t id2 = data2->id;

				if (forceTable[id1][id2] == 0.0f) continue;

				Vector2 distanceVector = { pos2->x - pos1->x, pos2->y - pos1->y };
				float distance = sqrtf(distanceVector.x * distanceVector.x + distanceVector.y * distanceVector.y);

				if (0.0f < distance && distance < maxDistance)
				{
					if (distance < radius)
					{
						v1->x = (radius - distance) / radius * MAXIMUM_FORCE * MAXIMUM_FORCE * deltaTime * distanceVector.x;
						v1->y = (radius - distance) / radius * MAXIMUM_FORCE * MAXIMUM_FORCE * deltaTime * distanceVector.y;
					}
					else if (radius < distance && distance <= halfDistance)
					{
						v1->x = (distance - radius) / halfDistance * forceTable[id1][id2] * deltaTime * distanceVector.x;
						v1->y = (distance - radius) / halfDistance * forceTable[id1][id2] * deltaTime * distanceVector.y;
					}
					else
					{
						v1->x = (maxDistance - distance) / halfDistance * forceTable[id1][id2] * deltaTime * distanceVector.x;
						v1->y = (maxDistance - distance) / halfDistance * forceTable[id1][id2] * deltaTime * distanceVector.y;
					}
				}
			}
		}
		
		// Update position
		for (Entity entity : scene->scope<ParticleData>())
		{
			ParticleData* data = scene->getComponent<ParticleData>(entity);
			Vector2* pos = &data->position;
			Vector2* v = &data->velocity;

			pos->x += v->x * deltaTime;
			pos->y += v->y * deltaTime;

			if (pos->x < simulationBorders.x || pos->x >(simulationBorders.x + simulationBorders.width))
			{
				v->x *= -1.0f;
				pos->x += 2 * v->x * deltaTime;
			}
			if (pos->y < simulationBorders.y || pos->y >(simulationBorders.y + simulationBorders.height))
			{
				v->y *= -1.0f;
				pos->y += 2 * v->y * deltaTime;
			}
		}

		// Draw
		BeginDrawing();

		ClearBackground(DARKGRAY);
		
		// Draw particles
		for (Entity entity : scene->scope<ParticleData, Sprite>())
		{
			ParticleData* data = scene->getComponent<ParticleData>(entity);
			Vector2* pos = &data->position;

			Sprite* spr = scene->getComponent<Sprite>(entity);

			DrawCircle(pos->x, pos->y, spr->radius, spr->color);
		}

		// UI 
		DrawText(TextFormat("Entity count : [%d]", scene->size()), 20, 20, 10, RAYWHITE);
		DrawText("Time scale : ", 20, 40, 10, RAYWHITE);
		GuiSlider(Rectangle(GetTextWidth("Time scale : ") + 20, 40, 100, 10), nullptr, TextFormat("%.3f", timeScale), &timeScale, -0.1f, 10.0f);

		float yPos = 60.0f;

		for (size_t i = 0; i < 4; i++)
		{
			DrawText("Particle ", 20, yPos, 10, RAYWHITE);
			DrawRectangle(20 + GetTextWidth("Particle "), yPos, 10, 10, colors[i]);
			yPos += 20.0f;
			DrawText("Starting amount ", 20, yPos, 10, RAYWHITE);
			if(GuiValueBox(Rectangle(20 + GetTextWidth("Starting amount "), yPos, 50, 10), nullptr, (int*)&startingAmount[i], MINIMUM_AMOUNT, MAXIMUM_AMOUNT, startingAmountMode[i])) 
				startingAmountMode[i] = !startingAmountMode[i];
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

		if (GuiButton(Rectangle(20, yPos, GetTextWidth("Back to zeroes") + 10, 20), "Back to zeroes"))
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					forceTable[i][j] = 0.0f;
		}

		if (GuiButton(Rectangle(120, yPos, GetTextWidth("Random") + 10, 20), "Random"))
		{
			for (size_t i = 0; i < 4; i++)
			{
				startingAmount[i] = rand() % (MAXIMUM_AMOUNT - MINIMUM_AMOUNT) + MINIMUM_AMOUNT;
				for (size_t j = 0; j < 4; j++)
					forceTable[i][j] = static_cast<float>((rand() % (MAXIMUM_FORCE - MINIMUM_FORCE) + MINIMUM_FORCE));
			}
		}

		if (GuiButton(Rectangle(176, yPos, GetTextWidth("Reset scene") + 10, 20), "Reset scene"))
		{
			delete scene;
			scene = createScene(startingAmount, simulationBorders);
		}

		DrawRectangleLinesEx(uiBorders, 2, LIGHTGRAY);

		EndDrawing();
	}

	return EXIT_SUCCESS;
}

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
			data->radius = float(rand() % 3) + 2.0f;
			data->maxDistance = float(rand() % 300) + 100.0f;

			Sprite* spr = scene->assignComponent<Sprite>(newEntity);
			*spr = { colors[i], data->radius };
		}
	}

	return scene;
}