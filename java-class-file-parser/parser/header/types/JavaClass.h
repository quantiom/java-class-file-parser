#pragma once
#include <iostream>
#include <vector>
#include <optional>

#include "../defines.h"
#include "../utils/AttributeReader.h"
#include "../utils/ConstantPool.h"

#include "JavaField.h"
#include "JavaMethod.h"

#include "../attributes/JavaAttribute.h"

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

class JavaClass : public AttributeReader, public AttributeHolder {
public:
	JavaClass(std::vector<u1> bytes) : AttributeHolder(this), AttributeReader(this, bytes) {
		this->m_constant_pool = ConstantPool();
	};

	// parses the class file
	JavaClass* parse();

	// get the bytes of the class file (including modifications made)
	std::vector<u1> get_bytes();

	// getters
	ConstantPool get_constant_pool();
	const std::vector<JavaField*> get_fields();
	const std::vector<JavaMethod*> get_methods();

	// setters
	void set_minor_version(u2 new_minor_version);
	void set_major_version(u2 new_major_version);
	void set_access_flags(u2 new_access_flags);
	void set_super_class_idx(u2 new_class_idx);

	// todo: add_field(...)

private:
	void parse_constant_pool();
	void parse_interfaces();
	void parse_fields();
	void parse_methods();
	void parse_attributes();

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
	std::vector<JavaField*> m_fields;

	// methods
	std::vector<JavaMethod*> m_methods;
};