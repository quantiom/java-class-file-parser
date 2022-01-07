#include "../../header/attributes/ConstantValueAttribute.h"
#include "../../header/types/JavaClass.h"
#include "../../header/utils/ByteWriter.h"

ConstantValueAttribute* ConstantValueAttribute::parse() {
	this->m_constantvalue_index = this->read_u2();

	return this;
}

std::vector<u1> ConstantValueAttribute::get_bytes() {
	std::unique_ptr<ByteWriter> writer(new ByteWriter());

	writer->write_u2(this->m_constantvalue_index);

	return writer->m_bytes;
}