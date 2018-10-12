#include "LuaTalker.h"


namespace LUA
{

	LuaTalker * LUA::LuaTalker::GetInstance()
	{
		static LuaTalker m_talker;

		return &m_talker;
	}

	LuaTalker::LuaTalker()
	{
		this->L = luaL_newstate();
		luaL_openlibs(this->L);
		this->lua = new sol::state_view(this->L);
		this->lua->open_libraries(sol::lib::base);
	}

	LuaTalker::~LuaTalker()
	{
		delete this->lua;
		lua_close(this->L);
	}

	lua_State* LuaTalker::getLuaState()
	{
		return this->L;
	}

	sol::state_view LuaTalker::getSolState()
	{
		return *lua;
	}

	int LuaTalker::loadFile(const char * adress)
	{
		return luaL_loadfile(this->L, adress) || lua_pcall(this->L, 0, 0, 0);
	}

	sol::protected_function_result LuaTalker::runProtectedFunction(const char* function)
	{ 
		sol::state_view sv = *lua;
		sol::protected_function test = sv[function];
		sol::protected_function_result result = test();
		if (!result.valid())
		{
			sol::error err = result;
			std::cout << "Error msg: " << err.what() << std::endl;
			system("pause");
			return 0;
		}
		return result;
	}

	void LuaTalker::runScript(std::string script)
	{
		this->lua->script(script);
	}

}
