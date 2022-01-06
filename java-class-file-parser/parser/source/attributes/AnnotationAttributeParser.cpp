#include "../../header/attributes/AnnotationAttributeParser.h"
#include "../../header/types/JavaClass.h"

JavaAnnotation AnnotationAttributeParser::parse_annotation() {
	const auto type_index = this->read_u2();
	const auto num_element_value_pairs = this->read_u2();

	std::cout << "type index: " << this->m_java_class->get_constant_pool().get_string(type_index) << "\n";

	for (auto i = 0; i < num_element_value_pairs; i++) {
		const auto element_name_index = this->read_u2();
		
		AnnotationElementValue element_value;
		std::cout << "element name index: " << this->m_java_class->get_constant_pool().get_string(element_name_index) << "\n";
	}

	return JavaAnnotation{ {}, {} };
}
