#pragma once
#include "../defines.h"
#include "../attributes/ParsedAttribute.h"
#include "JavaType.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.6

struct JavaMethod : public AttributeHolder {
	JavaMethod(JavaClass* java_class, u2 access_flags, u2 name_index, u2 descriptor_index, std::vector<std::shared_ptr<ParsedAttribute>> attributes) 
		: AttributeHolder(java_class, attributes), m_access_flags(access_flags), m_name_index(name_index),
		m_descriptor_index(descriptor_index) {};

	const auto get_access_flags() { return this->m_access_flags; }
	const auto get_name_index() { return this->m_name_index; }
	const auto get_descriptor_index() { return this->m_descriptor_index; }

	const void set_access_flags(u2 access_flags) { this->m_access_flags = access_flags; }
	const void set_name_index(u2 name_index) { this->m_name_index = name_index; }
	const void set_descriptor_index(u2 descriptor_index) { this->m_descriptor_index = descriptor_index; }

private:
	u2 m_access_flags;
	u2 m_name_index;
	u2 m_descriptor_index;
};