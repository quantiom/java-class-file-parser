#pragma once
#include "JavaAttribute.h"
#include "AnnotationAttribute.h"
#include "../types/JavaAnnotation.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.
// works with RuntimeInvisibleAnnotationsAttribute as well: https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.17

struct RuntimeAnnotationsAttribute : public AnnotationAttribute {
	// used in the variant
	RuntimeAnnotationsAttribute() : AnnotationAttribute() {};

	RuntimeAnnotationsAttribute(JavaClass* java_class, std::shared_ptr<JavaAttribute> attribute) 
		: AnnotationAttribute(java_class, attribute) {};

	RuntimeAnnotationsAttribute(JavaClass* java_class, u2 name_index, std::vector<u1> info)
		: AnnotationAttribute(java_class, name_index, info) {};

	void parse();
	std::vector<u1> get_bytes();

	const auto is_visible() { return this->m_is_visible; }
	const auto get_annotations() { return this->m_annotations; }
	void add_annotation(JavaAnnotation* annotation);
	void remove_annotation(const std::string& name);

private:
	bool m_is_visible; // false if this is a RuntimeInvisibleAnnotationsAttribute 
	std::vector<JavaAnnotation*> m_annotations;
};

struct RuntimeVisibleAnnotationsAttribute : public RuntimeAnnotationsAttribute {
	// used in the variant
	RuntimeVisibleAnnotationsAttribute() : RuntimeAnnotationsAttribute() {};

	RuntimeVisibleAnnotationsAttribute(JavaClass* java_class, std::shared_ptr<JavaAttribute> attribute) 
		: RuntimeAnnotationsAttribute(java_class, attribute) {};

	RuntimeVisibleAnnotationsAttribute(JavaClass* java_class, u2 name_index, std::vector<u1> info)
		: RuntimeAnnotationsAttribute(java_class, name_index, info) {};
};

struct RuntimeInvisibleAnnotationsAttribute : public RuntimeAnnotationsAttribute {
	// used in the variant
	RuntimeInvisibleAnnotationsAttribute() : RuntimeAnnotationsAttribute() {};

	RuntimeInvisibleAnnotationsAttribute(JavaClass* java_class, std::shared_ptr<JavaAttribute> attribute)
		: RuntimeAnnotationsAttribute(java_class, attribute) {
	};

	RuntimeInvisibleAnnotationsAttribute(JavaClass* java_class, u2 name_index, std::vector<u1> info)
		: RuntimeAnnotationsAttribute(java_class, name_index, info) {
	};
};