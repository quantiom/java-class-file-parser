#pragma once
#include "JavaType.h"
#include "../attributes/ParsedAttribute.h"

#include <optional>

class AttributeHolder : public JavaType {
public:
	AttributeHolder(
		JavaClass* java_class, 
		std::vector<std::shared_ptr<ParsedAttribute>> attributes
	) : JavaType(java_class), m_attributes(attributes) {};

	AttributeHolder(JavaClass* java_class) : JavaType(java_class) {};

	const auto get_attributes() { return this->m_attributes; }

	template<typename T>
	inline std::optional<std::shared_ptr<ParsedAttribute>> get_attribute() {
		const auto ret = std::find_if(this->m_attributes.begin(), this->m_attributes.end(),
			[](auto& attribute) -> bool { return std::get_if<T>(&(*attribute)) != nullptr; });

		if (ret != this->m_attributes.end()) {
			return *ret;
		} else {
			return {};
		}
	}

	template<typename T>
	inline void remove_attribute() {
		this->m_attributes.erase(
			std::remove_if(this->m_attributes.begin(), this->m_attributes.end(), [](auto& attribute) -> bool { return std::get_if<T>(&(*attribute)) != nullptr; }),
			this->m_attributes.end()
		);
	}

protected:
	std::vector<std::shared_ptr<ParsedAttribute>> m_attributes;
};