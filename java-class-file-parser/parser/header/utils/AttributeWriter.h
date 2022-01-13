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
			std::visit([this](auto& attribute) {
				this->write_u2(attribute.get_name_index());
				this->write_u4((u4)attribute.get_info().size());

				for (const auto& byte : attribute.get_info()) {
					this->write_u1(byte);
				}
			}, *attribute_ptr);
		}
	}
};