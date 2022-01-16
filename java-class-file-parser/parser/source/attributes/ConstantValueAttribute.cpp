#include "../../header/attributes/ConstantValueAttribute.h"
#include "../../header/types/JavaClass.h"
#include "../../header/utils/ByteWriter.h"

void ConstantValueAttribute::parse(std::unique_ptr<ByteReader>& reader) {
	this->m_constantvalue_index = reader->read_u2();
}

std::vector<u1> ConstantValueAttribute::get_bytes() {
	const auto writer = std::make_unique<ByteWriter>(ByteWriter());

	writer->write_u2(this->m_constantvalue_index);

	return writer->get_bytes();
}