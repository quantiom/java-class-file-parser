#include "../../header/utils/ByteReader.h"
#include "../../header/attributes/JavaAttribute.h"

JavaAttribute* ByteReader::read_attribute() {
	const auto attribute_name_index = this->read_u2();
	const auto attribute_length = this->read_u4();

	std::vector<u1> attribute_info;

	for (auto k = 0; k < attribute_length; k++) {
		attribute_info.push_back(this->read_u1());
	}

	return new JavaAttribute(this->m_java_class, attribute_name_index, attribute_info);
}
