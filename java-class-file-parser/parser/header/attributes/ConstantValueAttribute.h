#pragma once
#include "JavaAttribute.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.2

struct ConstantValueAttribute : public JavaAttribute {
	ConstantValueAttribute(JavaClass* java_class, JavaAttribute* attribute)
		: JavaAttribute(java_class, attribute) {};

	ConstantValueAttribute* parse();
	std::vector<u1> get_bytes();

	// index of the constant value in the constant pool
	u2 m_constantvalue_index;
};