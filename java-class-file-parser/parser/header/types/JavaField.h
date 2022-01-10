#pragma once
#include "JavaType.h"
#include "../defines.h"
#include "../attributes/ParsedAttribute.h"
#include "../types/JavaAnnotation.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.5

struct JavaField : public JavaType {
	JavaField(JavaClass* java_class, u2 access_flags, u2 name_index, u2 descriptor_index, std::vector<std::shared_ptr<ParsedAttribute>> attributes) 
		: JavaType(java_class), m_access_flags(access_flags), m_name_index(name_index), m_descriptor_index(descriptor_index), m_attributes(attributes) {
		for (const auto& attribute : this->m_attributes) {
			if (std::get_if<DeprecatedAttribute>(&(*attribute)) != nullptr) {
				this->m_deprecated = true;
			}
		}
	};

	const auto get_access_flags() { return this->m_access_flags; }
	const auto get_name_index() { return this->m_name_index; }
	const auto get_descriptor_index() { return this->m_descriptor_index; }
	const auto get_attributes() { return this->m_attributes; }

	std::string get_name();
	std::string get_descriptor();

	bool is_final();

	bool is_deprecated();
	void set_deprecated(bool new_value);

	// constant value index in the constant pool
	// only works if the field is final
	size_t get_constant_value_index();

	template <typename T>
	std::optional<std::shared_ptr<ParsedAttribute>> get_attribute();

	template <typename T>
	void remove_attribute();

	const std::vector<JavaAnnotation*> get_annotations(bool runtime_visible);
	const void add_annotation(JavaAnnotation* annotation, bool runtime_visible);
	const void remove_annotation(const std::string& name, bool runtime_visible);

private:
	u2 m_access_flags;
	u2 m_name_index;
	u2 m_descriptor_index;

	std::vector<std::shared_ptr<ParsedAttribute>> m_attributes;

	bool m_deprecated = false;
};
