#pragma once

class CheetParser
{
private:
	static Player * playerPtr;
	static EnemyHandler * enemyHandlerPtr;

	static bool m_visabilityDisable;
	static bool m_DrawCollisionBoxes;

	static bool m_DBG_CAM;
public:
	//Create instance for the singleton
	static CheetParser * GetInstance();

	//Parser input and execute correct stuff
	static void ParseString(const std::string & input);

	static bool SetPlayerCheetPointer(Player * p);
	static bool SetEnemyHandlerCheetPointer(EnemyHandler * en);

	static bool GetVisabilityDisabled();
	static bool GetDrawCollisionBoxes();

	static bool GetDBG_CAM();

	//Help function to display all the commmands
	static void _Help();
private:
	//Cheat classes.
	//This is where we put all the switch cases to avoid clutter
	static void _PlayerCheets();
	static void _SpawnCheets();
	static void _CommonCheets();
	static void _EngineCheets();
	

	static void _Draw();

	
};
