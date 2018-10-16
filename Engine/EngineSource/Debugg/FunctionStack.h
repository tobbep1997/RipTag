#pragma once
#include <stack>

namespace FunctionPush
{
	template <class t, typename r>
	class Stack
	{
		r _Execute(void (t::*func)(), t* c)
		{
			return (*c.*func)();
		}

		//Stack of functions.
		//This will handle the functions
		std::stack<r(t::*)()> stack;
	public:

		r Execute(t * type)
		{
			if (!Empty())
			{
				return _Execute(stack.top(), type);
			}
			return r();
		}
		bool Empty()
		{
			return stack.empty();
		}
		void Push(void (t::*func)())
		{
			stack.push(func);
		}
		void Pop()
		{
			stack.pop();
		}
		r Top()
		{
			return stack.top();
		}
		size_t Size()
		{
			return stack.size();
		}
	};

}