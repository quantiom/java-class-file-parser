#pragma once

class JavaClass;

class JavaType {
public:
	JavaType(JavaClass* java_class) : m_java_class(java_class) {};

	const JavaClass* get_java_class() { return this->m_java_class; }

protected:
	JavaClass* m_java_class;
};