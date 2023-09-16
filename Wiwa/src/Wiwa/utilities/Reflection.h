#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4401)
#pragma warning(disable : 4311)
#pragma warning(disable : 26495)
#include <typeinfo>
#include <vector>
#include <string>

#include <functional>

#include <Wiwa/utilities/containers/Array.h>
#include <Wiwa/utilities/Hashing.h>
#define memberoffset(type, member) (int)&((type*)0)->member

struct Type {
	std::string name;
	size_t size;
	size_t hash;
	bool is_class;
	bool is_enum;
	bool is_array;
	bool is_function;
	int custom_id;

	std::function<void* ()> New;

	bool operator==(const Type* other) const { return Equals(other); }

	bool Equals(const Type* other) const { return hash == other->hash; }
};

struct ParamData {
	std::string name;
	size_t hash;
};

struct Func : public Type {
	std::string signature;
	ParamData returnType;
	std::vector<ParamData> params;
	void* func;
};

struct Field {
	const Type* type;
	std::string name;
	size_t offset;
};

struct Class : public Type {
	std::vector<Field> fields;
	size_t field_count = 0;
};

struct EnumMember {
	const char* name;
	int value;
};

struct Enum : public Type {
	std::vector<EnumMember> members;
	size_t member_count;
};

inline std::string ClearCppName(std::string cname)
{
	// Clear namespace
	size_t nspace = cname.find(':');

	if (nspace != cname.npos) {
		cname.erase(0, nspace + 2);
		return cname;
	}

	// Clear class
	size_t ptype = cname.find("class ");

	if (ptype != cname.npos) {
		cname.erase(0, 6);
		return cname;
	}

	// Clear struct
	ptype = cname.find("struct ");

	if (ptype != cname.npos) {
		cname.erase(0, 7);
		return cname;
	}

	// Clear enum
	ptype = cname.find("enum ");

	if (ptype != cname.npos) {
		cname.erase(0, 5);
		return cname;
	}

	return cname;
}

inline ParamData FindTypes(std::string signature, std::vector<ParamData>& params) {
	ParamData ret;

	// Find __cdecl
	size_t cdec = signature.find("__cdecl");

	// Find return data
	std::string retstr = signature.substr(0, cdec);
	signature.erase(0, cdec + 6);

	retstr = ClearCppName(retstr);

	size_t ind = retstr.find(' ');

	ret.name = retstr.substr(0, ind);

	retstr.erase(0, ind + 1);

	ind = retstr.find("ptr");

	if (ind != signature.npos) {
		ret.name += "*";
	}

	ret.hash = FNV1A_HASH(ret.name.c_str());

	// Find parameters data
	ind = signature.find('(');

	signature.erase(0, ind + 1);

	ind = signature.find(',');

	if (ind == signature.npos) {
		ind = signature.find(')');

		if (ind == 0) {
			ind = signature.npos;
		}
	}

	while (ind != signature.npos) {
		ParamData pd;

		pd.name = signature.substr(0, ind);
		pd.name = ClearCppName(pd.name);

		size_t ind2 = pd.name.find("ptr");

		if (ind2 != pd.name.npos) {
			ind2 = pd.name.find(' ');
			pd.name = pd.name.substr(0, ind2);
			pd.name += "*";
		}

		pd.hash = FNV1A_HASH(pd.name.c_str());

		signature.erase(0, ind + 1);

		ind = signature.find(',');

		if (ind == signature.npos) {
			ind = signature.find(')');
		}

		params.push_back(pd);
	}
	
	printf("%s\n", signature.c_str());

	return ret;
}

#define BASE_TYPE_BODY(T) \
	type.name = ClearCppName(typeid(T).name()); \
	type.size = sizeof(T); \
	type.hash = FNV1A_HASH(type.name.c_str()); \
	type.is_class = false; \
	type.is_enum = false; \
	type.is_array = false; \
	type.is_function = false; \
	type.New = []() -> void* { return new T(); }; \
	type.custom_id = 0;

// FUNCTIONS
template<class T> inline const Type* GetType_impl() {
	static Type type;

	BASE_TYPE_BODY(T);

	return &type;
}

template<class T> inline const Type* GetType() {
	static const Type* type = GetType_impl<T>();

	return type;
}

template<class T> inline const Class* GetClass() {
	return (const Class*)GetType<T>();
}

template<class T> inline const Enum* GetEnum() {
	return (const Enum*)GetType<T>();
}

// COMPILE-TIME FUNCTIONS
template<size_t c>
const Type* GetCompileType() {
	return NULL;
}

#define REGISTER_TYPE(type) template<> inline const Type* GetCompileType<__COUNTER__>(){ \
	return GetType<type>(); \
}

// Register function
#define REGISTER_FUNCTION(functype) REGISTER_TYPE(decltype(functype)); \
template<> inline const Type* GetType_impl<decltype(functype)>(){ \
	static Func type; \
	type.name = #functype; \
	type.signature = typeid(functype).name(); \
	type.returnType = FindTypes(type.signature, type.params); \
	type.hash = FNV1A_HASH(#functype); \
	type.is_class = false; \
	type.is_enum = false; \
	type.is_array = false; \
	type.is_function = true; \
	type.custom_id = 2; \
	type.func = functype; \
	return &type; \
}

// Register system
#define REGISTER_SYSTEM(rtype) REGISTER_TYPE(rtype); \
template<> inline const Type* GetType_impl<rtype>(){ \
	static Type type; \
	BASE_TYPE_BODY(rtype); \
	type.custom_id = 1; \
	return &type; \
}

// REFLECT STRUCT / CLASS
#define REFLECTION_BEGIN(rtype) REGISTER_TYPE(rtype); \
template<> inline const Type* GetType_impl<rtype>(){ \
	static Class type; \
	BASE_TYPE_BODY(rtype); \
	type.is_class = true; \
	type.field_count = 0; \
	rtype temp;

#define REFLECT_MEMBER(member) type.fields.emplace_back(); \
	type.fields[type.field_count].name = #member; \
	type.fields[type.field_count].type = GetType<decltype(((decltype(temp)*)0)->member)>(); \
	type.fields[type.field_count].offset = memberoffset(decltype(temp), member); \
	type.field_count++;

#define REFLECTION_END return &type; }

// REFLECT ENUMERATOR
#define ENUM_REFLECTION_BEGIN(rtype) \
template<> inline const Type* GetType_impl<rtype>(){ \
	static Enum type; \
	BASE_TYPE_BODY(rtype); \
	type.is_enum = true; \
	type.member_count = 0; \
	rtype temp;

#define ENUM_REFLECT_MEMBER(member) type.members.emplace_back(); \
	type.members[type.member_count].name = #member; \
	type.members[type.member_count].value = (int)decltype(temp)::member; \
	type.member_count++;

#define ENUM_REFLECTION_END REFLECTION_END

//======== Protect sub-apps from engine reflections ==========================================================================================
#ifdef WI_BUILD_DLL
#define WI_REFLECTION_BEGIN REFLECTION_BEGIN
#define WI_REFLECT_MEMBER REFLECT_MEMBER
#define WI_REFLECTION_END REFLECTION_END

#define WI_REGISTER_FUNCTION REGISTER_FUNCTION
#define WI_REGISTER_TYPE REGISTER_TYPE
#define WI_REGISTER_SYSTEM REGISTER_SYSTEM
#else
#define WI_REFLECTION_BEGIN(...)
#define WI_REFLECT_MEMBER(...)
#define WI_REFLECTION_END

#define WI_REGISTER_FUNCTION(...)
#define WI_REGISTER_TYPE(...)
#define WI_REGISTER_SYSTEM(...)
#endif

//======== Compile-time types ================================================================================================================

// Type count of objects in the engine
extern const size_t TYPE_COUNT;

#define TYPELIST const Wiwa::Array<const Type*, TYPE_COUNT>*

// No return foreach
template <size_t N, size_t I = 0>
void GetTypes_loop(Wiwa::Array<const Type*, N>& arr) {
	// Take type
	arr[I] = GetCompileType<I>();

	// Recurse from I to N
	if constexpr (I + 1 < N) GetTypes_loop<N, I + 1>(arr);
};

template <size_t count>
const Wiwa::Array<const Type*, count>* GetTypes_impl() {
	static Wiwa::Array<const Type*, count> types{};

	GetTypes_loop<count, 0>(types);

	return &types;
};

template<size_t count>
inline const Wiwa::Array<const Type*, count>* GetTypes() {
	static const Wiwa::Array<const Type*, count>* types = GetTypes_impl<count>();

	return types;
};

// Call where TYPE_COUNT is defined
#define GET_TYPES() GetTypes<TYPE_COUNT>()

// Enum of basic type hashes
enum class TypeHash : size_t {
	// Basic hashes
	Bool = FNV1A_HASH("bool"),
	Int32 = FNV1A_HASH("int"),
	UInt32 = FNV1A_HASH("unsigned int"),
	Int64 = FNV1A_HASH("__int64"),
	UInt64 = FNV1A_HASH("unsigned __int64"),
	Char = FNV1A_HASH("char"),
	UChar = FNV1A_HASH("unsigned char"),
	WChar = FNV1A_HASH("wchar_t"),
	Float = FNV1A_HASH("float"),
	Double = FNV1A_HASH("double"),
	LongDouble = FNV1A_HASH("long double"),
	LongLong = FNV1A_HASH("long long"),
	ULongLong = FNV1A_HASH("unsigned long long"),

	// Vector hashes
	Vector2i = FNV1A_HASH("Vector2i"),
	Vector2f = FNV1A_HASH("Vector2f"),

	Vector3i = FNV1A_HASH("Vector3i"),
	Vector3f = FNV1A_HASH("Vector3f"),

	// Rect hashes
	Rect2i = FNV1A_HASH("Rect2i"),

	// Custom
	Pivot = FNV1A_HASH("Pivot"),

	// Components
	Transform3D = FNV1A_HASH("Transform3D"),
	Mesh = FNV1A_HASH("Mesh"),
	AudioSource = FNV1A_HASH("AudioSource"),
	PointLight = FNV1A_HASH("PointLight"),
	DirectionalLight = FNV1A_HASH("DirectionalLight"),
	SpotLight = FNV1A_HASH("SpotLight"),
	CollisionBody = FNV1A_HASH("CollisionBody"),
	AnimatorComponent = FNV1A_HASH("AnimatorComponent"),
	ColliderCube = FNV1A_HASH("ColliderCube"),
	ColliderCylinder = FNV1A_HASH("ColliderCylinder"),
	ColliderSphere = FNV1A_HASH("ColliderSphere"),
	ColliderCapsule = FNV1A_HASH("ColliderCapsule"),
	RayCast = FNV1A_HASH("RayCast"),
	Item = FNV1A_HASH("Item"),
	NavMesh = FNV1A_HASH("NavMesh"),
	NavAgent = FNV1A_HASH("NavAgent"),

	// Game Controller
	Character = FNV1A_HASH("Character"),
	Player = FNV1A_HASH("Player"),
	PlayerSpawner = FNV1A_HASH("PlayerSpawnerData"),
	Wave = FNV1A_HASH("Wave"),
	ParticleEmitter = FNV1A_HASH("ParticleEmitterComponent"),
	WaveSpawner = FNV1A_HASH("WaveSpawner"),
	EnemyState = FNV1A_HASH("EnemyState"),
	EnemyData = FNV1A_HASH("EnemyData"),
	Health = FNV1A_HASH("Health")

};