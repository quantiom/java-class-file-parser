#pragma once
#include "JavaAttribute.h"
#include "../types/JavaAnnotation.h"

class AnnotationAttributeParser : public BaseAttributeParser {
public:
	AnnotationAttributeParser(JavaClass* java_class, JavaAttribute* attribute)
		: BaseAttributeParser(java_class, attribute) {};

	// parse an annotation at the current byte index
	JavaAnnotation* parse_annotation();

	// parse an annotation element value at the current byte index
	AnnotationElementValue* parse_element_value();
};