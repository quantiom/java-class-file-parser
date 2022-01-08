#include "../../header/types/JavaClass.h"
#include "../../header/types/JavaField.h"

#include "../../header/attributes/RuntimeAnnotationsAttribute.h"
#include "../../header/attributes/ConstantValueAttribute.h"

std::string JavaField::get_name() {
	return this->m_java_class->get_constant_pool().get_string(this->m_name_index);
}

std::string JavaField::get_descriptor() {
	return this->m_java_class->get_constant_pool().get_string(this->m_descriptor_index);
}

bool JavaField::is_final() {
	return this->m_access_flags & (int)AccessFlags::ACC_FINAL;
}

bool JavaField::is_deprecated() {
	return this->m_deprecated;
}

// TODO: cleanup
void JavaField::set_deprecated(bool new_value) {
	if (this->m_deprecated == new_value)
		return;

	if (new_value) {
		this->add_annotation(new JavaAnnotation(this->m_java_class, this->m_java_class->get_constant_pool().get_or_add_utf8("Ljava/lang/Deprecated;"), {}));
		this->m_attributes.push_back(std::make_shared<ParsedAttribute>(JavaAttribute(this->m_java_class, this->m_java_class->get_constant_pool().get_or_add_utf8("Deprecated"), {})));
	} else {
		for (auto it = this->m_attributes.begin(); it != this->m_attributes.end(); it++) {
			auto attribute_ptr = *it;

			if (std::get_if<DeprecatedAttribute>(&(*attribute_ptr)) != nullptr) {
				this->m_attributes.erase(it--);
			} else if (const auto runtime_annotations = std::get_if<RuntimeAnnotationsAttribute>(&(*attribute_ptr)); runtime_annotations != nullptr) {
				runtime_annotations->remove_annotation("Ljava/lang/Deprecated;");
			}
		}
	}

	this->m_deprecated = new_value;
}

size_t JavaField::get_constant_value_index() {
	if (!this->is_final()) {
		throw std::runtime_error("This field is not constant.");
	}

	for (const auto& attribute : this->m_attributes) {
		if (auto constant_value_attribute = std::get_if<ConstantValueAttribute>(&(*attribute)); constant_value_attribute != nullptr) {
			return constant_value_attribute->get_constantvalue_index();
		}
	}

	return 0;
}

const std::vector<JavaAnnotation*> JavaField::get_annotations() {
	std::vector<JavaAnnotation*> annotations;
	
	for (auto& attribute : this->m_attributes) {
		if (auto runtime_annotations_attribute = std::get_if<RuntimeAnnotationsAttribute>(&(*attribute)); runtime_annotations_attribute != nullptr) {
			annotations.insert(annotations.end(), runtime_annotations_attribute->get_annotations().begin(), runtime_annotations_attribute->get_annotations().end());
		}
	}

	return annotations;
}

const void JavaField::add_annotation(JavaAnnotation* annotation) {
	bool found_runtime_annotations_attribute = false;

	for (auto& attribute : this->m_attributes) {
		if (auto runtime_annotations_attribute = std::get_if<RuntimeAnnotationsAttribute>(&(*attribute)); runtime_annotations_attribute != nullptr) {
			runtime_annotations_attribute->add_annotation(annotation);
			found_runtime_annotations_attribute = true;
		}
	}

	if (!found_runtime_annotations_attribute) {
		this->m_attributes.push_back(std::make_shared<ParsedAttribute>(RuntimeAnnotationsAttribute(this->m_java_class, this->m_java_class->get_constant_pool().get_or_add_utf8("RuntimeVisibleAnnotations"), {})));
		this->add_annotation(annotation);
	}
}

const void JavaField::remove_annotation(const std::string& name) {
	for (auto& attribute : this->m_attributes) {
		if (auto runtime_annotations_attribute = std::get_if<RuntimeAnnotationsAttribute>(&(*attribute)); runtime_annotations_attribute != nullptr) {
			runtime_annotations_attribute->remove_annotation(name);
		}
	}
}
