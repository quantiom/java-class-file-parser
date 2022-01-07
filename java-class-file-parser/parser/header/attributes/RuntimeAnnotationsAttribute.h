#pragma once
#include "JavaAttribute.h"
#include "AnnotationAttribute.h"
#include "../types/JavaAnnotation.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.
// works with RuntimeInvisibleAnnotationsAttribute as well: https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.17

struct RuntimeAnnotationsAttribute : public AnnotationAttribute {
	RuntimeAnnotationsAttribute(JavaClass* java_class, JavaAttribute* attribute)
		: AnnotationAttribute(java_class, attribute) {};

	RuntimeAnnotationsAttribute* parse();
	std::vector<u1> get_bytes();

	// annotations
	std::vector<JavaAnnotation*> m_annotations;
};