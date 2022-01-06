#pragma once
#include <vector>
#include "../defines.h"
#include "../utils/ByteReader.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7

class JavaClass;

struct JavaAttribute {
	u2 m_name_index;
	u4 m_length;
	std::vector<u1> m_info;
};

class BaseAttributeParser : public JavaAttribute, public ByteReader {
public:
	BaseAttributeParser(JavaClass* java_class, const JavaAttribute attribute) 
		: ByteReader(java_class, attribute.m_info) {
		this->m_name_index = attribute.m_name_index;
		this->m_length = attribute.m_length;
		this->m_info = attribute.m_info;
	};

	BaseAttributeParser(JavaClass* java_class) : ByteReader(java_class, this->m_info) {};
};