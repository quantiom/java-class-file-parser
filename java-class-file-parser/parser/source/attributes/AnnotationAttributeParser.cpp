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
		element_value->m_annotation_value = this->parse_annotation();
	} else if (element_value->is_array_value()) { // Array
		element_value->m_array_value.m_element_values.push_back(this->parse_element_value());
	}

	return element_value;
}

void AnnotationAttributeParser::get_annotation_bytes(std::unique_ptr<ByteWriter>& writer, JavaAnnotation* annotation) {
	writer->write_u2(annotation->m_type_index);
	writer->write_u2(annotation->m_element_value_pairs.size());

	for (const auto& [name_index, element_value] : annotation->m_element_value_pairs) {
		writer->write_u2(name_index);
		this->get_element_value_bytes(writer, element_value);
	}
}

void AnnotationAttributeParser::get_element_value_bytes(std::unique_ptr<ByteWriter>& writer, AnnotationElementValue* element_value) {
	if (element_value->is_const_value()) { // Const Value
		writer->write_u2(element_value->m_const_value_index);
	} else if (element_value->is_enum_const()) { // Enum Constant
		writer->write_u2(element_value->m_enum_const_value.m_const_name_index);
		writer->write_u2(element_value->m_enum_const_value.m_type_name_index);
	} else if (element_value->is_class_info_index()) { // Class Info
		writer->write_u2(element_value->m_class_info_index);
	} else if (element_value->is_annotation_value()) { // Nested Annotation
		this->get_annotation_bytes(writer, element_value->m_annotation_value);
	} else if (element_value->is_array_value()) { // Array
		for (const auto& nested_element_value : element_value->m_array_value.m_element_values) {
			this->get_element_value_bytes(writer, nested_element_value);
		}
	}
}
