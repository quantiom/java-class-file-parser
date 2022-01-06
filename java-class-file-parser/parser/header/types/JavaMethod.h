#pragma once
#include "../defines.h"
#include "../attributes/JavaAttribute.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.6

struct JavaMethod {
	u2 m_access_flags;
	u2 m_name_index;
	u2 m_descriptor_index;
	std::vector<JavaAttribute*> m_attributes;
};