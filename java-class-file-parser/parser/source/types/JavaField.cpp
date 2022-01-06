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
		this->m_attributes.push_back(new JavaAttribute(this->m_java_class, this->m_java_class->get_constant_pool().get_or_add_utf8("Deprecated"), {}));
	} else {
		for (auto it = this->m_attributes.begin(); it != this->m_attributes.end(); it++) {
			const auto attribute = *it;
			const auto attribute_name = attribute->get_name();

			if (attribute_name == "Deprecated") {
				delete attribute;
				this->m_attributes.erase(it--);
			} else if (attribute_name == "RuntimeVisibleAnnotations") {
				std::unique_ptr<RuntimeAnnotationsAttribute> runtime_annotation_attribute(new RuntimeAnnotationsAttribute(this->m_java_class, attribute));
				runtime_annotation_attribute->parse();

				for (auto annotation_it = runtime_annotation_attribute->m_annotations.begin(); annotation_it != runtime_annotation_attribute->m_annotations.end(); annotation_it++) {
					const auto annotation = *annotation_it;

					if (annotation->get_name() == "Ljava/lang/Deprecated;") {
						runtime_annotation_attribute->m_annotations.erase(annotation_it--);
					}
				}

				if (runtime_annotation_attribute->m_annotations.empty()) {
					delete attribute;
					this->m_attributes.erase(it--);
				} else {
					attribute->m_info = runtime_annotation_attribute->get_bytes();
				}
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
		const auto attribute_name = attribute->get_name();

		if (attribute_name == "ConstantValue") {
			return ConstantValueAttribute(this->m_java_class, attribute).parse()->m_constantvalue_index;
		}
	}

	return 0;
}

const std::vector<JavaAnnotation*> JavaField::get_annotations() {
	std::vector<JavaAnnotation*> annotations;
	
	for (const auto& attribute : this->m_attributes) {
		const auto attribute_name = attribute->get_name();

		if (attribute_name == "RuntimeVisibleAnnotations" || attribute_name == "RuntimeInvisibleAnnotations") {
			std::unique_ptr<RuntimeAnnotationsAttribute> attribute_annotations(new RuntimeAnnotationsAttribute(this->m_java_class, attribute));
			attribute_annotations->parse();

			annotations.insert(annotations.end(), attribute_annotations->m_annotations.begin(), attribute_annotations->m_annotations.end());
		}
	}

	return annotations;
}
