#pragma once

namespace Fn {
	class CallableF {};

	template<class _ret, class... _params>
	class Function {
		typedef _ret(*s_funtype)(_params...);
	private:
		_ret(CallableF::* func)(_params...);
		CallableF* obj;
		void* s_func;
	public:
		Function() {
			obj = NULL;
			s_func = NULL;
		}

		template<class T>
		Function(_ret(T::* _func)(_params...), void* _obj) {
			func = (_ret(CallableF::*)(_params...))_func;
			obj = (CallableF*)_obj;
		}

		Function(void* fun) {
			obj = NULL;
			s_func = fun;
		}

		_ret execute(_params... params) {
			if (obj) {
				return (obj->*func)(params...);
			}

			return ((s_funtype)s_func)(params...);
		}

		_ret operator()(_params... params) {
			return execute(params...);
		}

		void* getObj() { return obj; }
		void* getFuncPtr() { return obj == NULL ? (void*&)func : s_func; }
	};
}