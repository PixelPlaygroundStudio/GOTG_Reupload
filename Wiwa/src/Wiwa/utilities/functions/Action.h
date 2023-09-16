#pragma once

class Callable {};

template<class... _params>
class Action {
private:
	void (Callable::* action)(_params...);
	Callable* obj;
	void* func;
public:	
	template<class T>
	Action() {
		obj = NULL;
	}

	Action() {
		obj = NULL;
		func = NULL;
	}

	Action(void* _func) {
		obj = NULL;
		func = _func;
	}

	template<class T>
	Action(void (T::* _action)(_params...), void* _obj) {
		action = (void (Callable::*)(_params...))_action;
		obj = (Callable*)_obj;
	}

	void execute(_params... params) {
		if (obj) {
			(obj->*action)(params...);
		}
		else {
			void (*f)(_params...) = (void(*)(_params...))func;

			f(params...);
		}
	}

	void operator()(_params... params) {
		execute(params...);
	}

	bool operator==(Action<> other) {
		return obj == other.obj
			&& getActionPtr() == other.getActionPtr();
	}

	void* getObj() { return obj; }
	void* getActionPtr() { return obj == NULL ? func : (void*&)action; }
};