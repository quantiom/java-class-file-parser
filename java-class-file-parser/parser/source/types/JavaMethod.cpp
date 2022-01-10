#include "../../header/types/JavaMethod.h"
#include "../../header/types/JavaClass.h"

std::string JavaMethod::get_name() {
	return this->m_java_class->get_constant_pool().get_string(this->m_name_index);
}

std::string JavaMethod::get_descriptor() {
	return this->m_java_class->get_constant_pool().get_string(this->m_descriptor_index);
}
