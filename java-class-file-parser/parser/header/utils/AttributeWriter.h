#pragma once
#include "ByteWriter.h"
#include "../attributes/ParsedAttribute.h"

class AttributeWriter : public ByteWriter {
public:
	void write_attributes(std::vector<std::shared_ptr<ParsedAttribute>> attributes) {
		this->write_u2((u2)attributes.size());

		// TODO: make attributes only store their parsed data so it doesn't use
		// double the memory
		for (const auto& attribute_ptr : attributes) {
			this->write_attribute(attribute_ptr);
		}
	}

	void write_attribute(std::shared_ptr<ParsedAttribute> attribute_ptr) {
		std::visit([this](auto& attribute) {
			auto bytes = attribute.get_bytes();

			this->write_u2(attribute.get_name_index());
			this->write_u4((u4)bytes.size());

			for (const auto& byte : bytes) {
				this->write_u1(byte);
			}
		}, *attribute_ptr);
	}
};