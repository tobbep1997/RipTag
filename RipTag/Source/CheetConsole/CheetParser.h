#pragma once

class CheetParser
{
private:
	static Player * playerPtr;
	static EnemyHandler * enemyHandlerPtr;

	static bool m_visabilityDisable;
	static bool m_DrawCollisionBoxes;
public:
	static CheetParser * GetInstance();

	static void ParseString(const std::string & input);

	static bool SetPlayerCheetPointer(Player * p);
	static bool SetEnemyHandlerCheetPointer(EnemyHandler * en);

	static bool GetVisabilityDisabled();
	static bool GetDrawCollisionBoxes();

	static void _Help();
private:

	static void _PlayerCheets();
	static void _SpawnCheets();
	static void _CommonCheets();
	static void _Draw();

	
};
