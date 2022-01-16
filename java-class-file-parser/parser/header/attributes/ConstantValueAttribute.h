#pragma once
#include "JavaAttribute.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.2

class ConstantValueAttribute : public JavaAttribute {
public:
	ConstantValueAttribute() : JavaAttribute() {};
	ConstantValueAttribute(JavaClass* java_class, std::shared_ptr<JavaAttribute> attribute) : JavaAttribute(java_class, attribute) {};
	ConstantValueAttribute(JavaClass* java_class, u2 name_index) : JavaAttribute(java_class, name_index) {};

	void parse(std::unique_ptr<ByteReader>& reader);
	std::vector<u1> get_bytes();

	const auto get_constantvalue_index() { return this->m_constantvalue_index; }

private:
	// index of the constant value in the constant pool
	u2 m_constantvalue_index;
};