#pragma once
#include "JavaAttribute.h"
#include "../types/JavaClass.h"

struct ConstantValueAttribute : public BaseAttributeParser {
	ConstantValueAttribute(const JavaAttribute attribute, JavaClass* java_class) 
		: BaseAttributeParser(attribute, java_class) {};

	ConstantValueAttribute* parse();

	// index of the constant value in the constant pool
	u2 m_constantvalue_index;
};