#pragma once
#include "../defines.h"

#include <vector>
#include <optional>

#include <unordered_map>

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
	void add_constant(ConstantPoolEntryInfo entry);

	std::optional<ConstantPoolEntryInfo> get_entry(size_t idx);

	std::string get_string(size_t idx);
	std::string get_method_type(size_t idx);
	ConstantPoolReference get_reference(size_t idx);
	std::pair<std::string, std::string> get_name_and_type(size_t idx);

	u4 get_integer(size_t idx);
	float get_float(size_t idx);
	double get_double(size_t idx);
	long long get_long(size_t idx);

	// gets the index of a UTF8 string from the constant pool
	// or adds it if it does not exist
	size_t get_or_add_utf8(std::string str);

	// gets the index of an integer from the constant pool
	// or adds it if it does not exist
	size_t get_or_add_integer(u4 integer);

	std::vector<ConstantPoolEntryInfo> get_entries();
	size_t get_size();
private:
	// cache
	std::unordered_map<std::string, size_t> m_cached_strings;
	std::unordered_map<u4, size_t> m_cached_integers;
	
	std::vector<ConstantPoolEntryInfo> m_entries;
};