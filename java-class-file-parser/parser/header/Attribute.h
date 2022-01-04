#pragma once
#include <vector>
#include "defines.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7

struct Attribute {
	u2 m_name_index;
	u4 m_length;
	std::vector<u1> m_info;
};