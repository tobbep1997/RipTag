#pragma once

class CheatParser
{
private:
	static Player * playerPtr;
	static EnemyHandler * enemyHandlerPtr;

	static bool m_visabilityDisable;
	static bool m_DrawCollisionBoxes;

	static bool m_DBG_CAM;
public:
	//Create instance for the singleton
	static CheatParser * GetInstance();

	//Parser input and execute correct stuff
	static void ParseString(const std::string & input);

	static bool SetPlayerCheatPointer(Player * p);
	static bool SetEnemyHandlerCheatPointer(EnemyHandler * en);

	static bool GetVisabilityDisabled();
	static bool GetDrawCollisionBoxes();

	static bool GetDBG_CAM();

	//Help function to display all the commmands
	static void _Help();
private:
	//Cheat classes.
	//This is where we put all the switch cases to avoid clutter
	static void _PlayerCheats();
	static void _SpawnCheats();
	static void _CommonCheats();
	static void _EngineCheats();
	

	static void _Draw();

	
};
