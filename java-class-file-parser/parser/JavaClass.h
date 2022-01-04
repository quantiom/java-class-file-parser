#pragma once
#include <iostream>
#include <vector>
#include <optional>

#include "defines.h"
#include "ConstantPool.h"

class JavaClass {
public:
	JavaClass(std::vector<uint8_t> bytes) : m_bytes(bytes) {
		this->m_constant_pool = ConstantPool();
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

	ConstantPool m_constant_pool;

	size_t m_current_byte_index;
	std::vector<u1> m_bytes;
};