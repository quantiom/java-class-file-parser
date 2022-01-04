#pragma once
#include <iostream>
#include <vector>

using u4 = uint32_t;
using u2 = uint16_t;
using u1 = uint8_t;

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
	u1 tag;
	std::vector<u1> info;
};

class JavaClass {
public:
	JavaClass(std::vector<uint8_t> bytes) : m_bytes(bytes) {
		this->m_current_byte_index = 0;
	};

	void parse();

private:
	void parse_constant_pool();

	u4 read_u4();
	u2 read_u2();
	u1 read_u1();

	u4 read_u4(size_t start_idx);
	u2 read_u2(size_t start_idx);
	u1 read_u1(size_t idx);

	u2 m_minor_version;
	u2 m_major_version;

	std::vector<ConstantPoolEntryInfo> m_constant_pool;

	size_t m_current_byte_index;
	std::vector<u1> m_bytes;
};