#include "JavaClass.h"

void JavaClass::parse() {
	if (this->read_u1(0) != 0xCA || this->read_u1(1) != 0xFE || this->read_u1(2) != 0xBA || this->read_u1(3) != 0xBE) {
		throw std::invalid_argument("Invalid java class file. Does not have 0xCAFEBABE magic.");
	}

	this->m_minor_version = this->read_u2(4);
	this->m_major_version = this->read_u2(6);

	this->parse_constant_pool();
}

void JavaClass::parse_constant_pool() {
	uint32_t constant_pool_count = this->read_u2(8);

	const uint32_t start = 10;
	uint32_t offset = 0;

	for (int i = 1; i < constant_pool_count; i++) {
		const auto tag = (ConstantPoolType)this->read_u1(start + offset);

		switch (tag) {
			case ConstantPoolType::CONSTANT_Methodref:
				auto class_index = this->read_u2(start + offset + 1);
				auto name_and_type_index = this->read_u2(start + offset + 3);

				std::cout << "class index: " << (int)class_index << '\n';
				std::cout << "name and type index: " << (int)name_and_type_index << '\n';

				break;
		}

		break;
	}
}

uint32_t JavaClass::read_u4(uint32_t start_idx) {
	return this->m_bytes[start_idx] + this->m_bytes[start_idx + 1] + this->m_bytes[start_idx + 2] + this->m_bytes[start_idx + 3];
}

uint16_t JavaClass::read_u2(uint32_t start_idx) {
	return this->m_bytes[start_idx] + this->m_bytes[start_idx + 1];
}

uint32_t JavaClass::read_u1(uint32_t idx) {
	return this->m_bytes[idx];
}
