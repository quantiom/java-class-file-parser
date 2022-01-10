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

void JavaField::set_deprecated(bool new_value) {
	if (this->m_deprecated == new_value)
		return;

	if (new_value) {
		this->add_annotation(std::make_shared<JavaAnnotation>(JavaAnnotation(this->m_java_class, this->m_java_class->get_constant_pool().get_or_add_utf8("Ljava/lang/Deprecated;"), {})), true);
		this->m_attributes.push_back(std::make_shared<ParsedAttribute>(JavaAttribute(this->m_java_class, this->m_java_class->get_constant_pool().get_or_add_utf8("Deprecated"), {})));
	} else {
		this->remove_attribute<DeprecatedAttribute>();
		this->remove_annotation("Ljava/lang/Deprecated;", true);
	}

	this->m_deprecated = new_value;
}

std::optional<size_t> JavaField::get_constant_value_index() {
	if (auto constant_value_attribute = this->get_attribute<ConstantValueAttribute>()) {
		return std::get<ConstantValueAttribute>(**constant_value_attribute).get_constantvalue_index();
	}

	return {};
}

const std::vector<std::shared_ptr<JavaAnnotation>> JavaField::get_annotations(bool runtime_visible) {
	if (runtime_visible) {
		if (auto attribute = this->get_attribute<RuntimeVisibleAnnotationsAttribute>())
			return std::get<RuntimeVisibleAnnotationsAttribute>(**attribute).get_annotations();
	} else {
		if (auto attribute = this->get_attribute<RuntimeInvisibleAnnotationsAttribute>())
			return std::get<RuntimeVisibleAnnotationsAttribute>(**attribute).get_annotations();
	}

	return {};
}

const void JavaField::add_annotation(std::shared_ptr<JavaAnnotation> annotation, bool runtime_visible) {
	std::shared_ptr<ParsedAttribute> attribute;

	if (runtime_visible) {
		auto found_attribute = this->get_attribute<RuntimeVisibleAnnotationsAttribute>();

		if (!found_attribute) {
			found_attribute = std::make_shared<ParsedAttribute>(RuntimeVisibleAnnotationsAttribute(this->m_java_class, this->m_java_class->get_constant_pool().get_or_add_utf8("RuntimeVisibleAnnotations"), {}));
		}

		attribute = found_attribute.value();

		this->m_attributes.push_back(attribute);
		std::get<RuntimeVisibleAnnotationsAttribute>(*attribute).add_annotation(annotation);
	} else {
		auto found_attribute = this->get_attribute<RuntimeInvisibleAnnotationsAttribute>();

		if (found_attribute == nullptr) {
			found_attribute = std::make_shared<ParsedAttribute>(RuntimeInvisibleAnnotationsAttribute(this->m_java_class, this->m_java_class->get_constant_pool().get_or_add_utf8("RuntimeVisibleAnnotations"), {}));
		}

		attribute = found_attribute.value();

		this->m_attributes.push_back(attribute);
		std::get<RuntimeInvisibleAnnotationsAttribute>(*attribute).add_annotation(annotation);
	}
}

const void JavaField::remove_annotation(const std::string& name, bool runtime_visible) {
	if (runtime_visible) {
		if (const auto found_attribute = this->get_attribute<RuntimeVisibleAnnotationsAttribute>(); found_attribute != nullptr) {
			std::get<RuntimeVisibleAnnotationsAttribute>(**found_attribute).remove_annotation(name);
		}
	} else {
		const auto found_attribute = this->get_attribute<RuntimeInvisibleAnnotationsAttribute>();
		
		if (const auto found_attribute = this->get_attribute<RuntimeInvisibleAnnotationsAttribute>(); found_attribute != nullptr) {
			std::get<RuntimeInvisibleAnnotationsAttribute>(**found_attribute).remove_annotation(name);
		}
	}
}