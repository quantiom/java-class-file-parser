#pragma once
#include <iostream>
#include <vector>

#include "ConstantPool.h"

class JavaClass {
public:
	JavaClass(std::vector<uint8_t> bytes) : m_bytes(bytes) {};

	void parse();

private:
	void parse_constant_pool();

	uint32_t read_u4(uint32_t start_idx);
	uint16_t read_u2(uint32_t start_idx);
	uint32_t read_u1(uint32_t idx);

	uint16_t m_minor_version;
	uint16_t m_major_version;

	std::vector<uint8_t> m_bytes;
};