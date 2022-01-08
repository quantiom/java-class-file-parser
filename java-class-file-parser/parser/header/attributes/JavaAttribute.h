#pragma once
#include <vector>
#include "../defines.h"
#include "../utils/ByteReader.h"

#include "../types/JavaType.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7

struct JavaAttribute : public ByteReader {
	// used in the variant
	JavaAttribute() : ByteReader(nullptr, {}) {};
	
	JavaAttribute(JavaClass* java_class, u2 name_index, std::vector<u1> info)
		: ByteReader(java_class, info), m_name_index(name_index), m_info(info) {};

	JavaAttribute(JavaClass* java_class, std::shared_ptr<JavaAttribute>& attribute)
		: JavaAttribute(java_class, attribute->m_name_index, attribute->m_info) {};

	u2 m_name_index;
	std::vector<u1> m_info;

	// utility methods/fields

	virtual void parse() {};
	virtual std::vector<u1> get_bytes() { return this->m_info; };

	std::string get_name();
};