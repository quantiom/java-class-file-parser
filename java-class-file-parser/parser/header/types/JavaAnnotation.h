#pragma once
#include <vector>
#include <map>

#include "../defines.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.3

struct JavaAnnotation;

union AnnotationElementValueUnion {
	u2 m_const_value_index;

	struct {
		u2 m_type_name_index;
		u2 m_const_name_index;
	} m_enum_const_value;

	u2 m_class_info_index;

	std::vector<JavaAnnotation*> m_annotation_value;

	struct {
		std::vector<AnnotationElementValue> m_element_values;
	} m_array_value;
};

struct AnnotationElementValue {
	u1 m_tag;
	AnnotationElementValueUnion m_value;
};

struct JavaAnnotation {
	u2 m_type_index;
	u2 m_num_element_value_pairs;

	// descriptor index - element value
	std::map<u2, AnnotationElementValue*> m_element_value_pairs;
};