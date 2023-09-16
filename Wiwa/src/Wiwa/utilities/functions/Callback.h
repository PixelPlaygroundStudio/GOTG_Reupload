#pragma once

#include <string>
#include <vector>
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class Callback {
	protected:
		std::string m_Name;
		size_t m_Hash;

		std::vector<const Type*> m_Params;
	public:
		std::string getName() { return m_Name; }
		size_t getHash() { return m_Hash; }
		
		size_t getParamCount() { return m_Params.size(); }
		const Type* getParamAt(size_t index) { return m_Params[index]; }

		Callback();
		
		virtual ~Callback() = default;

		virtual void Execute(void** params=NULL){}
	};
}