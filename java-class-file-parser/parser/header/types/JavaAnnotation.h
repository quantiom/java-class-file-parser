#pragma once
#include <vector>

#include "../defines.h"
#include "JavaType.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.3

struct JavaAnnotation;
struct AnnotationElementValue;

// TODO: convert to variant (I think - since it's tagged it can't be a union)
struct AnnotationElementValue {
	AnnotationElementValue(u1 tag) : m_tag(tag) {};
	
	u1 m_tag;
	
	// only of these values are used
	u2 m_const_value_index;

	struct {
		u2 m_type_name_index;
		u2 m_const_name_index;
	} m_enum_const_value;

	u2 m_class_info_index;

	std::shared_ptr<JavaAnnotation> m_annotation_value;

	struct {
		std::vector<std::shared_ptr<AnnotationElementValue>> m_element_values;
	} m_array_value;

	bool is_const_value() {
		const std::vector<char> const_value_tags = { 'B', 'C', 'D', 'F', 'I', 'J', 'S', 'Z', 's' };
		return std::find(const_value_tags.begin(), const_value_tags.end(), this->m_tag) != const_value_tags.end();
	}

	bool is_enum_const() {
		return this->m_tag == 'e';
	}

	bool is_class_info_index() {
		return this->m_tag == 'c';
	}

	bool is_annotation_value() {
		return this->m_tag == '@';
	}

	bool is_array_value() {
		return this->m_tag == '[';
	}
};

struct JavaAnnotation : public JavaType {
	JavaAnnotation(JavaClass* java_class, u2 type_index, std::vector<std::pair<u2, std::shared_ptr<AnnotationElementValue>>> element_value_pairs)
		: JavaType(java_class), m_type_index(type_index), m_element_value_pairs(element_value_pairs) {};

	const std::string get_name();
	const auto get_type_index() { return this->m_type_index; }
	auto get_element_value_pairs() { return this->m_element_value_pairs; }

private:
	u2 m_type_index;

	// descriptor index - element value
	std::vector<std::pair<u2, std::shared_ptr<AnnotationElementValue>>> m_element_value_pairs;
};