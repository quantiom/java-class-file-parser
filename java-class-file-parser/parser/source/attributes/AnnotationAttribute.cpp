#include "../../header/attributes/AnnotationAttribute.h"
#include "../../header/types/JavaClass.h"

std::shared_ptr<JavaAnnotation> AnnotationAttribute::parse_annotation(std::unique_ptr<ByteReader>& reader) {
	const auto type_index = reader->read_u2();
	const auto num_element_value_pairs = reader->read_u2();

	std::vector<std::pair<u2, std::shared_ptr<AnnotationElementValue>>> element_value_pairs;

	for (auto i = 0; i < num_element_value_pairs; i++) {
		auto element_name_index = reader->read_u2();
		auto element_value = this->parse_element_value(reader);

		element_value_pairs.push_back(std::make_pair<>(element_name_index, element_value));
	}

	return std::make_shared<JavaAnnotation>(this->m_java_class, type_index, element_value_pairs);
}

std::shared_ptr<AnnotationElementValue> AnnotationAttribute::parse_element_value(std::unique_ptr<ByteReader>& reader) {
	const auto element_value_tag = reader->read_u1();

	auto element_value = std::make_shared<AnnotationElementValue>(element_value_tag);

	if (element_value->is_const_value()) { // Const Value
		element_value->m_const_value_index = reader->read_u2();
	} else if (element_value->is_enum_const()) { // Enum Constant
		element_value->m_enum_const_value.m_const_name_index = reader->read_u2();
		element_value->m_enum_const_value.m_type_name_index = reader->read_u2();
	} else if (element_value->is_class_info_index()) { // Class Info
		element_value->m_class_info_index = reader->read_u2();
	} else if (element_value->is_annotation_value()) { // Nested Annotation
		element_value->m_annotation_value = this->parse_annotation(reader);
	} else if (element_value->is_array_value()) { // Array
		element_value->m_array_value.m_element_values.push_back(this->parse_element_value(reader));
	}

	return element_value;
}

void AnnotationAttribute::get_annotation_bytes(std::unique_ptr<ByteWriter>& writer, std::shared_ptr<JavaAnnotation> annotation) {
	writer->write_u2(annotation->get_type_index());
	writer->write_u2((u2)annotation->get_element_value_pairs().size());

	for (auto& [name_index, element_value] : annotation->get_element_value_pairs()) {
		writer->write_u2(name_index);
		this->get_element_value_bytes(writer, element_value);
	}
}

void AnnotationAttribute::get_element_value_bytes(std::unique_ptr<ByteWriter>& writer, std::shared_ptr<AnnotationElementValue>& element_value) {
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
		for (auto& nested_element_value : element_value->m_array_value.m_element_values) {
			this->get_element_value_bytes(writer, nested_element_value);
		}
	}
}
