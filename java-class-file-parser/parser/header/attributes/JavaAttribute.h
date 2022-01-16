#pragma once
#include <vector>
#include "../defines.h"
#include "../utils/ByteReader.h"

#include "../types/JavaType.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7

class JavaAttribute {
public:
	JavaAttribute() {};
	JavaAttribute(JavaClass* java_class, u2 name_index) : m_java_class(java_class), m_name_index(name_index) {};
	JavaAttribute(JavaClass* java_class, std::shared_ptr<JavaAttribute>& attribute) : JavaAttribute(java_class, attribute->m_name_index) {};

	virtual void parse(std::unique_ptr<ByteReader>& reader) {};
	virtual std::vector<u1> get_bytes() { return {}; };

	std::string get_name();
	const auto get_name_index() { return this->m_name_index; }

protected:
	JavaClass* m_java_class;
	u2 m_name_index;
};