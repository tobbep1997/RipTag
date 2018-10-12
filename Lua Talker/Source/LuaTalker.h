#pragma once
#include <iostream>
#include "../Lua Talker/Lua Externals/sol.hpp"
namespace LUA
{
	
	class LuaTalker
	{
	public:
		static LuaTalker * GetInstance();
		LuaTalker();
		~LuaTalker();
		lua_State* getLuaState();
		sol::state_view getSolState();


		int loadFile(const char * adress);
		sol::protected_function_result runProtectedFunction(const char* function);
		void runScript(std::string script);

		template <typename Key, typename T> void defineObjectToLua(Key&& name, sol::usertype<T>& object);
		template <typename Key, typename T> void addReferenceToLua(Key&& name, T& object);
	private:
		lua_State *L;
		sol::state_view* lua;
	};



	template<typename Key, typename T>
	inline void LuaTalker::defineObjectToLua(Key && name, sol::usertype<T>& object)
	{
		this->lua->set_usertype(name, object);
	}

	template<typename Key, typename T>
	inline void LuaTalker::addReferenceToLua(Key && name, T & object)
	{
		this->lua->set(name, &object);
	}

}
