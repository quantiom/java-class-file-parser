#pragma once

class JavaClass;

class JavaType {
public:
	JavaType(JavaClass* java_class) : m_java_class(java_class) {};

	JavaClass* m_java_class;
};