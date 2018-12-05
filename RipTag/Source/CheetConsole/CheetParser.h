#pragma once

class CheetParser
{
private:
	static Player * playerPtr;
public:
	static CheetParser * GetInstance();

	static void ParseString(const std::string & input);

	static bool SetPlayerCheetPointer(Player * p);
	
};
