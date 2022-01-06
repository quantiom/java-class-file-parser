#include "../../header/attributes/AnnotationAttributeParser.h"
#include "../../header/types/JavaClass.h"

JavaAnnotation* AnnotationAttributeParser::parse_annotation() {
	const auto type_index = this->read_u2();
	const auto num_element_value_pairs = this->read_u2();

	std::vector<std::pair<u2, AnnotationElementValue*>> element_value_pairs;

	for (auto i = 0; i < num_element_value_pairs; i++) {
		auto element_name_index = this->read_u2();
		auto element_value = this->parse_element_value();

		element_value_pairs.push_back(std::make_pair<>(element_name_index, element_value));
	}

	return new JavaAnnotation(this->m_java_class, type_index, element_value_pairs);
}

AnnotationElementValue* AnnotationAttributeParser::parse_element_value() {
	const auto element_value_tag = this->read_u1();

	AnnotationElementValue* element_value = new AnnotationElementValue(element_value_tag);

	if (element_value->is_const_value()) { // Const Value
		element_value->m_const_value_index = this->read_u2();
	} else if (element_value->is_enum_const()) { // Enum Constant
		element_value->m_enum_const_value.m_const_name_index = this->read_u2();
		element_value->m_enum_const_value.m_type_name_index = this->read_u2();
	} else if (element_value->is_class_info_index()) { // Class Info
		element_value->m_class_info_index = this->read_u2();
	} else if (element_value->is_annotation_value()) { // Nested Annotation
		element_value->m_annotation_value.push_back(this->parse_annotation());
	} else if (element_value->is_array_value()) { // Array
		element_value->m_array_value.m_element_values.push_back(this->parse_element_value());
	}

	return element_value;
}
