#pragma once
#include <iostream>
#include <vector>
#include <optional>

#include "defines.h"
#include "ConstantPool.h"
#include "JavaField.h"
#include "Attribute.h"

// Oracle Reference: https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.1

enum class AccessFlags {
	ACC_PUBLIC = 0x0001,
	ACC_PRIVATE = 0x0002,
	ACC_PROTECTED = 0x0004,
	ACC_STATIC = 0x0008,
	ACC_FINAL = 0x0010,
	ACC_VOLATILE = 0x0040,
	ACC_TRANSIENT = 0x0080,
	ACC_SYNTHETIC = 0x1000,
	ACC_ENUM = 0x4000,
};

class JavaClass {
public:
	JavaClass(std::vector<u1> bytes) : m_bytes(bytes) {
		this->m_constant_pool = ConstantPool();
		this->m_current_byte_index = 0;
	};

	// parses the class file
	void parse();

private:
	void parse_constant_pool();
	void parse_interfaces();
	void parse_fields();
	void parse_methods();

	// parse an attribute at the current byte index
	Attribute parse_attribute();
	
	// methods for reading bytes from the file
	u4 read_u4();
	u2 read_u2();
	u1 read_u1();

	u4 read_u4(size_t start_idx);
	u2 read_u2(size_t start_idx);
	u1 read_u1(size_t idx);

	// version the class was compiled for
	u2 m_minor_version;
	u2 m_major_version;

	// constant pool wrapper
	ConstantPool m_constant_pool;

	// access flags for the class (AccessFlags)
	u2 m_access_flags;

	// index of this and super class in the constant pool
	u2 m_this_class_idx;
	u2 m_super_class_idx;

	// index of all interfaces this classes implements in the constant pool (to CONSTANT_Class-es)
	std::vector<u2> m_interfaces;

	// fields
	std::vector<JavaField> m_fields;

	// bytes of the class file
	size_t m_current_byte_index;
	std::vector<u1> m_bytes;
};