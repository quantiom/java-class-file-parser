#pragma once
#include "JavaAttribute.h"
#include "../types/JavaAnnotation.h"
#include "../utils/ByteWriter.h"

class AnnotationAttribute : public JavaAttribute {
public:
	AnnotationAttribute(JavaClass* java_class, JavaAttribute* attribute)
		: JavaAttribute(java_class, attribute) {};

	// parse an annotation at the current byte index
	JavaAnnotation* parse_annotation();

	// parse an annotation element value at the current byte index
	AnnotationElementValue* parse_element_value();

	// write an annotation into a ByteWriter
	void get_annotation_bytes(std::unique_ptr<ByteWriter>& writer, JavaAnnotation* annotation);

	// write an annotation element value into a ByteWriter
	void get_element_value_bytes(std::unique_ptr<ByteWriter>& writer, AnnotationElementValue* annotation);
};