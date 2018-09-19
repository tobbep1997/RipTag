#pragma once
#include <Vector>

namespace FunctionPush
{
	template <class t, typename r>
	class Vector
	{
		r _Execute(void (t::*func)(), t* c)
		{
			return (*c.*func)();
		}

		//Stack of functions.
		//This will handle the functions
		std::vector<r(t::*)()> vector;
	public:

		r Execute(t * type, int i = 0)
		{
			if (!Empty())
			{
				return _Execute(vector.at(i), type);
			}
			return r();
		}
		bool Empty()
		{
			return vector.empty();
		}
		void Push(void (t::*func)())
		{
			vector.push_back(func);
		}
		void Pop()
		{
			vector.pop_back();
		}
		r Top()
		{
			return vector.front();
		}
		size_t Size()
		{
			return vector.size();
		}
	};

}
