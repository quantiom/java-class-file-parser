#include "../../header/types/JavaAnnotation.h"
#include "../../header/types/JavaClass.h"

const std::string JavaAnnotation::get_name() {
	return this->m_java_class->get_constant_pool()->get_string(this->m_type_index);
}