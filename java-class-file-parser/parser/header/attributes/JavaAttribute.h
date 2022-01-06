#pragma once
#include <vector>
#include "../defines.h"
#include "../utils/ByteReader.h"

#include "../types/JavaType.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7

struct JavaAttribute : public ByteReader {
	JavaAttribute(JavaClass* java_class, u2 name_index, u2 length, std::vector<u1> info)
		: ByteReader(java_class, info), m_name_index(name_index), m_length(length), m_info(info) {};

	u2 m_name_index;
	u4 m_length;
	std::vector<u1> m_info;

	//

	std::string get_name() {
		return this->m_java_class->get_constant_pool().get_string(this->m_name_index);
	}
};

struct BaseAttributeParser : public JavaAttribute {
	BaseAttributeParser(JavaClass* java_class, JavaAttribute* attribute)
		: JavaAttribute(java_class, attribute->m_name_index, attribute->m_length, attribute->m_info) {};
};