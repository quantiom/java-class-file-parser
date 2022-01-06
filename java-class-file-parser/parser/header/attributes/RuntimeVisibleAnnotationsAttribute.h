#pragma once
#include "JavaAttribute.h"
#include "AnnotationAttributeParser.h".h"
#include "../types/JavaAnnotation.h"

struct RuntimeVisibleAnnotationsAttribute : public AnnotationAttributeParser {
	RuntimeVisibleAnnotationsAttribute(JavaClass* java_class, const JavaAttribute attribute)
		: AnnotationAttributeParser(java_class, attribute) {};

	RuntimeVisibleAnnotationsAttribute* parse();

	// annotations
	std::vector<JavaAnnotation*> m_annotations;
};