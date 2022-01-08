#pragma once
#include "JavaAttribute.h"
#include "AnnotationAttribute.h"
#include "../types/JavaAnnotation.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.
// works with RuntimeInvisibleAnnotationsAttribute as well: https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.17

struct RuntimeAnnotationsAttribute : public AnnotationAttribute {
	// used in the variant
	RuntimeAnnotationsAttribute() : AnnotationAttribute() {};

	RuntimeAnnotationsAttribute(JavaClass* java_class, std::shared_ptr<JavaAttribute> attribute) : AnnotationAttribute(java_class, attribute) {};

	RuntimeAnnotationsAttribute(JavaClass* java_class, u2 name_index, std::vector<u1> info)
		: AnnotationAttribute(java_class, name_index, info) {};

	void parse();
	std::vector<u1> get_bytes();

	bool m_is_visible; // false if this is a RuntimeInvisibleAnnotationsAttribute 
	std::vector<JavaAnnotation*> m_annotations;
};