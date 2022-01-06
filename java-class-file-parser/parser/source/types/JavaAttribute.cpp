#include "../../header/attributes/JavaAttribute.h"
#include "../../header/types/JavaClass.h"

std::string JavaAttribute::get_name() {
	return this->m_java_class->get_constant_pool().get_string(this->m_name_index);
}
