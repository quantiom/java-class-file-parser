#pragma once
#include "JavaAttribute.h"
#include "AnnotationAttributeParser.h"
#include "../types/JavaAnnotation.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.
// works with RuntimeInvisibleAnnotationsAttribute as well: https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.17

struct RuntimeAnnotationsAttribute : public AnnotationAttributeParser {
	RuntimeAnnotationsAttribute(JavaClass* java_class, JavaAttribute* attribute)
		: AnnotationAttributeParser(java_class, attribute) {};

	RuntimeAnnotationsAttribute* parse();
	std::vector<u1> get_bytes();

	// annotations
	std::vector<JavaAnnotation*> m_annotations;
};