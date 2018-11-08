#pragma once
#include <vector>
#include "../Actors/Enemy/Enemy.h"

class Grid;

class EnemyHandler
{
private:
	std::vector<Enemy*> m_guards;
	Grid * m_grid;

public:
	EnemyHandler();
	~EnemyHandler();

	// Add guard function rather than init for guards (?)
	void Init(std::vector<Enemy*> enemies, Grid * grid);
	
	void Update(float deltaTime);

private:
	
};