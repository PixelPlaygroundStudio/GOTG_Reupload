#pragma once
#include <memory>
#ifdef WI_PLATFORM_WINDOWS
	#ifdef WI_BUILD_DLL 
		#define WI_API __declspec(dllexport)
	#else
		#define WI_API __declspec(dllimport)
	#endif
#else
	#error WIWA ONLY SUPPORTS WINDOWS!
#endif

#ifdef WI_ENABLE_ASSERTS
	#define WI_ASSERT(x, ...) {if(!(x)) { WI_ERROR("Assertion Failed: {0}", __VA_ARGS__);  __debugbreak(); } }
	#define WI_ASSERT_MSG(...) { WI_ERROR("Assertion Failed: {0}", __VA_ARGS__);  __debugbreak(); } 
	#define WI_CORE_ASSERT(x, ...) {if(!(x)) { WI_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define WI_CORE_ASSERT_MSG(...) { WI_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}
#else
	#define WI_ASSERT(x, ...) 
	#define WI_ASSERT_MSG(...)
	#define WI_CORE_ASSERT(x, ...)
	#define WI_CORE_ASSERT_MSG(...)
#endif


#define BIT(x) (1 << (x))
#define CLAMP(x, mn, mx) (x) < (mn) ? (x) = (mn) : (x) = (x); (x) > (mx) ? (x) = (mx) : (x) = (x)

#define RAND(min, max) rand() % (max - min + 1) + min
#define RANDF(min, max)  (max - min) * ((((float) rand()) / (float) RAND_MAX)) + min
#define IN_BETWEEN(value, min, max) value >= min && value <= max
#define IS_DROP_RATE(value, min, max) value >= min && value <= (min + max)

#define PI 3.14159265359
#define PI_F 3.14159265359f

#define WI_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
#define WI_HARD_INL __forceinline
constexpr unsigned long long WI_INVALID_INDEX = -1;

namespace Wiwa {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}