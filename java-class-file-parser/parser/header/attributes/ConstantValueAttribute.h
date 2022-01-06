#pragma once
#include "JavaAttribute.h"

struct ConstantValueAttribute : public BaseAttributeParser {
	ConstantValueAttribute(JavaClass* java_class, JavaAttribute* attribute)
		: BaseAttributeParser(java_class, attribute) {};

	ConstantValueAttribute* parse();

	// index of the constant value in the constant pool
	u2 m_constantvalue_index;
};