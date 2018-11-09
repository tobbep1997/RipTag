#pragma once
#include <vector>
//#include "../Actors/Enemy/Enemy.h"

class Enemy;
class Player;
class Grid;

class EnemyHandler
{
private:
	const float SOUND_LEVEL = 0.33f;
	const int SIGHT_LEVEL = 1700;

private:
	std::vector<Enemy*> m_guards;
	Player * m_player;
	Grid * m_grid;

public:
	EnemyHandler();
	~EnemyHandler();

	// Add guard function rather than init for guards (?)
	void Init(std::vector<Enemy*> enemies, Player * player, Grid * grid);
	
	void Update(float deltaTime);

private:
	int _getPlayerVisibility(Enemy * guard);
	void _alert(Enemy * guard, bool followSound = false);
	void _investigating(Enemy * guard, int playerVisibility);
	void _investigateSound(Enemy * guard);
	void _patrolling(Enemy * guard, int playerVisibility);

};