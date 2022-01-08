#pragma once
#include "JavaAttribute.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.2

struct ConstantValueAttribute : public JavaAttribute {
	// used in the variant
	ConstantValueAttribute() : JavaAttribute() {};

	ConstantValueAttribute(JavaClass* java_class, std::shared_ptr<JavaAttribute> attribute) : JavaAttribute(java_class, attribute) {};

	ConstantValueAttribute(JavaClass* java_class, u2 name_index, std::vector<u1> info)
		: JavaAttribute(java_class, name_index, info) {};

	void parse();
	std::vector<u1> get_bytes();

	// index of the constant value in the constant pool
	u2 m_constantvalue_index;
};