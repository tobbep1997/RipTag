#pragma once

class CheetParser
{
private:
	static Player * playerPtr;
	static EnemyHandler * enemyHandlerPtr;
public:
	static CheetParser * GetInstance();

	static void ParseString(const std::string & input);

	static bool SetPlayerCheetPointer(Player * p);
	static bool SetEnemyHandlerCheetPointer(EnemyHandler * en);
private:

	static void _PlayerCheets();
	static void _SpawnCheets();
};
