
#include <random>
#include <iostream>

struct position {
	float x, y, z;
};

struct scale {
	float x, y, z;
};

void printPosition(position* pos)
{
	std::cout << pos->x << '\t' << pos->y << '\t' << pos->z << '\n';
}

void printScale(scale* scale)
{
	std::cout << scale->x << '\t' << scale->y << '\t' << scale->z << '\n';
}

#include "ecs/scene.h"

int main()
{
	Scene scene(12);
	/*
	Entity zorik = scene.createEntity();
	scene.getComponent<float>(zorik);
	//scene.destroyEntity(zorik);
	position* zorikPosition = scene.assignComponent<position>(zorik);
	scale* zorikScale = scene.assignComponent<scale>(zorik);
	
	*zorikPosition = position{ 13.0f, 3.0f, 7.0f };
	*zorikScale = scale{ 14.0f, 4.0f, 8.0f };
	
	printPosition(zorikPosition);
	*/

	for (size_t i = 0; i < 15; i++)
	{
		Entity temp = scene.createEntity();
		position* tempPos = scene.assignComponent<position>(temp);
		*tempPos = { float(std::rand() % 999)/10.0f, float(std::rand() % 999) / 10.0f , float(std::rand() % 999) / 10.0f };
		//printPosition(tempPos);
	}
	
	Scope scope = scene.scope<position>();

	for (auto ENTT : scene.scope<position>())
	{
		position* tempPos = scene.getComponent<position>(ENTT);
		printPosition(tempPos);
	}
	
	return 0;
}