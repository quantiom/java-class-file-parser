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
		this->m_attributes.push_back(std::make_shared<ParsedAttribute>(JavaAttribute(this->m_java_class, this->m_java_class->get_constant_pool().get_or_add_utf8("Deprecated"), {})));
	} else {
		for (auto it = this->m_attributes.begin(); it != this->m_attributes.end(); it++) {
			auto attribute_ptr = *it;

			// TODO: fix
			/*std::visit([this, it](auto& attribute) {
				const auto attribute_name = attribute.get_name();

				if (attribute_name == "Deprecated") {
					this->m_attributes.erase(it--);
				} else if (attribute_name == "RuntimeVisibleAnnotations") {
					auto runtime_annotations_attribute = std::dynamic_pointer_cast<RuntimeAnnotationsAttribute>(attribute);

					for (auto annotation_it = runtime_annotations_attribute->m_annotations.begin(); annotation_it != runtime_annotations_attribute->m_annotations.end(); annotation_it++) {
						const auto annotation = *annotation_it;

						if (annotation->get_name() == "Ljava/lang/Deprecated;") {
							runtime_annotations_attribute->m_annotations.erase(annotation_it--);
						}
					}

					if (runtime_annotations_attribute->m_annotations.empty()) {
						this->m_attributes.erase(it--);
					} else {
						attribute->m_info = runtime_annotations_attribute->get_bytes();
					}
				}
			}, *attribute_ptr);*/
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
			return constant_value_attribute->m_constantvalue_index;
		}
	}

	return 0;
}

const std::vector<JavaAnnotation*> JavaField::get_annotations() {
	std::vector<JavaAnnotation*> annotations;
	
	for (auto& attribute : this->m_attributes) {
		if (auto runtime_annotations_attribute = std::get_if<RuntimeAnnotationsAttribute>(&(*attribute)); runtime_annotations_attribute != nullptr) {
			annotations.insert(annotations.end(), runtime_annotations_attribute->m_annotations.begin(), runtime_annotations_attribute->m_annotations.end());
		}
	}

	return annotations;
}
