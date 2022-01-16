#pragma once
#include "JavaAttribute.h"
#include "../types/JavaAnnotation.h"
#include "../utils/ByteWriter.h"

class AnnotationAttribute : public JavaAttribute {
public:
	AnnotationAttribute() : JavaAttribute() {};
	AnnotationAttribute(JavaClass* java_class, std::shared_ptr<JavaAttribute>& attribute) : JavaAttribute(java_class, attribute) {};
	AnnotationAttribute(JavaClass* java_class, u2 name_index) : JavaAttribute(java_class, name_index) {};

	// parse an annotation at the current byte index
	std::shared_ptr<JavaAnnotation> parse_annotation(std::unique_ptr<ByteReader>& reader);

	// parse an annotation element value at the current byte index
	std::shared_ptr<AnnotationElementValue> parse_element_value(std::unique_ptr<ByteReader>& reader);

	// write an annotation into a ByteWriter
	void get_annotation_bytes(std::unique_ptr<ByteWriter>& writer, std::shared_ptr<JavaAnnotation> annotation);

	// write an annotation element value into a ByteWriter
	void get_element_value_bytes(std::unique_ptr<ByteWriter>& writer, std::shared_ptr<AnnotationElementValue>& annotation);
};