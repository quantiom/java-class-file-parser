#include "../../header/attributes/ConstantValueAttribute.h"
#include "../../header/types/JavaClass.h"

ConstantValueAttribute* ConstantValueAttribute::parse() {
	this->m_constantvalue_index = this->read_u2();

	return this;
}
