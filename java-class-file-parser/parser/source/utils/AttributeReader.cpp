#include "../../header/types/JavaClass.h"
#include "../../header/utils/AttributeReader.h"

std::shared_ptr<ParsedAttribute> AttributeReader::read_attribute() {
	const auto attribute_name_index = this->read_u2();
	const auto attribute_length = this->read_u4();

	std::vector<u1> attribute_info;

	for (size_t k = 0; k < attribute_length; k++) {
		attribute_info.push_back(this->read_u1());
	}

	const auto name = this->m_java_class->get_constant_pool()->get_string(attribute_name_index);

	std::shared_ptr<ParsedAttribute> attribute;

	switch (hash::Fnv1a<uint32_t>(name)) {
		case hash::Fnv1a<uint32_t>("Code"):
			attribute = std::make_shared<ParsedAttribute>(CodeAttribute(this->m_java_class, attribute_name_index, attribute_info));
			break;
		case hash::Fnv1a<uint32_t>("Deprecated"):
			attribute = std::make_shared<ParsedAttribute>(DeprecatedAttribute(this->m_java_class, attribute_name_index, attribute_info));
			break;
		case hash::Fnv1a<uint32_t>("RuntimeInvisibleAnnotations"):
			attribute = std::make_shared<ParsedAttribute>(RuntimeInvisibleAnnotationsAttribute(this->m_java_class, attribute_name_index, attribute_info));
			break;
		case hash::Fnv1a<uint32_t>("RuntimeVisibleAnnotations"):
			attribute = std::make_shared<ParsedAttribute>(RuntimeVisibleAnnotationsAttribute(this->m_java_class, attribute_name_index, attribute_info));
			break;
		default:
			attribute = std::make_shared<ParsedAttribute>(JavaAttribute(this->m_java_class, attribute_name_index, attribute_info));
	}

	// parse the attribute with the class's parse() method
	std::visit([](auto& a) { a.parse(); }, *attribute);

	return attribute;
}
