#pragma once
#include "../defines.h"

#include <vector>
#include <optional>

#include <unordered_map>

class JavaClass;

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4

enum class ConstantPoolType {
	CONSTANT_Utf8 = 1,
	CONSTANT_Integer = 3,
	CONSTANT_Float = 4,
	CONSTANT_Long = 5,
	CONSTANT_Double = 6,
	CONSTANT_Class = 7,
	CONSTANT_String = 8,
	CONSTANT_Fieldref = 9,
	CONSTANT_Methodref = 10,
	CONSTANT_InterfaceMethodref = 11,
	CONSTANT_NameAndType = 12,
	CONSTANT_MethodHandle = 15,
	CONSTANT_MethodType = 16,
	CONSTANT_InvokeDynamic = 18
};

struct ConstantPoolEntryInfo {
	const u1 m_tag;
	const std::vector<u1> m_info;
};

struct ConstantPoolReference {
	const u2 m_class_index;
	const u2 m_name_and_type_index;
};

class ConstantPool {
public:
	void parse(JavaClass* java_class);

	void add_entry(ConstantPoolEntryInfo entry);

	std::optional<ConstantPoolEntryInfo> get_entry(size_t idx);

	std::string get_string(size_t idx);
	std::string get_method_type(size_t idx);
	ConstantPoolReference get_reference(size_t idx);
	std::pair<std::string, std::string> get_name_and_type(size_t idx);

	s4 get_integer(size_t idx);
	float get_float(size_t idx);
	double get_double(size_t idx);
	long long get_long(size_t idx);
	u2 get_class(size_t idx);

	u2 get_or_add_utf8(const std::string& str);
	u2 get_or_add_class(const std::string& str);
	u2 get_or_add_integer(s4 integer);

	std::vector<ConstantPoolEntryInfo> get_entries();
	size_t get_size();
private:
	// cache
	std::unordered_map<std::string, u2> m_cached_strings;
	std::unordered_map<u4, u2> m_cached_integers;
	std::unordered_map<std::string, u2> m_cached_classes;
	
	std::vector<ConstantPoolEntryInfo> m_entries;
};