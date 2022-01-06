#pragma once
#include "../defines.h"
#include <vector>

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.3

struct AnnotationElementValue {
	u1 m_tag;
	// TODO: this
};

struct JavaAnnotation {
	u2 m_type_index;
	u2 m_num_element_value_pairs;

	// descriptor index - element value
	std::vector<u2, AnnotationElementValue> m_element_value_pairs;
};