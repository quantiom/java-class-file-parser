#pragma once
#include "JavaType.h"
#include "../defines.h"
#include "../attributes/JavaAttribute.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.5

struct JavaField : public JavaType {
	JavaField(JavaClass* java_class, u2 access_flags, u2 name_index, u2 descriptor_index, std::vector<JavaAttribute*> attributes) 
		: JavaType(java_class), m_access_flags(access_flags), m_name_index(name_index), 
		m_descriptor_index(descriptor_index), m_attributes(attributes) {};

	u2 m_access_flags;
	u2 m_name_index;
	u2 m_descriptor_index;
	std::vector<JavaAttribute*> m_attributes;
};