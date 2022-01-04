#include <optional>
#include "JavaClass.h"

void JavaClass::parse() {
	if (this->read_u1() != 0xCA || this->read_u1() != 0xFE || this->read_u1() != 0xBA || this->read_u1() != 0xBE) {
		throw std::invalid_argument("Invalid java class file. Does not have 0xCAFEBABE magic.");
	}

	this->m_minor_version = this->read_u2();
	this->m_major_version = this->read_u2();

	this->parse_constant_pool();
}

void JavaClass::parse_constant_pool() {
	const auto constant_pool_count = this->read_u2();

	for (int i = 1; i < constant_pool_count; i++) {
		const auto tag = this->read_u1();
		std::optional<std::vector<u1>> byte_info;

		switch ((ConstantPoolType)tag) {
			case ConstantPoolType::CONSTANT_Fieldref:
			case ConstantPoolType::CONSTANT_InterfaceMethodref:
			case ConstantPoolType::CONSTANT_Methodref: {
				const auto class_index = this->read_u2();
				const auto name_and_type_index = this->read_u2();
				
				byte_info = { (u1)(class_index >> 8), (u1)class_index, (u1)(name_and_type_index >> 8), (u1)name_and_type_index };

				//std::cout << "class index: " << (int)class_index << '\n';
				//std::cout << "name and type index: " << (int)name_and_type_index << '\n';
			} break;
			case ConstantPoolType::CONSTANT_Class:
			case ConstantPoolType::CONSTANT_String: {
				const auto name_index = this->read_u2();

				byte_info = { (u1)(name_index >> 8), (u1)name_index };
			} break;
			case ConstantPoolType::CONSTANT_Integer:
			case ConstantPoolType::CONSTANT_Float: {
				const auto value = this->read_u2();
				const auto bytes = (u1*)&value;

				byte_info = { bytes[0], bytes[1], bytes[2], bytes[3] };
			} break;

		}

		if (byte_info.has_value()) {
			this->m_constant_pool.push_back(ConstantPoolEntryInfo{ tag, byte_info.value() });
		}
	}
}

u4 JavaClass::read_u4() {
	const auto ret = this->read_u4(this->m_current_byte_index);
	this->m_current_byte_index += 4;
	return ret;
}

u2 JavaClass::read_u2() {
	const auto ret = this->read_u2(this->m_current_byte_index);
	this->m_current_byte_index += 2;
	return ret;
}

u1 JavaClass::read_u1() {
	const auto ret = this->read_u1(this->m_current_byte_index);
	this->m_current_byte_index += 1;
	return ret;
}

u4 JavaClass::read_u4(size_t start_idx) {
	return this->m_bytes[start_idx] + this->m_bytes[start_idx + 1] + this->m_bytes[start_idx + 2] + this->m_bytes[start_idx + 3];
}

u2 JavaClass::read_u2(size_t start_idx) {
	return this->m_bytes[start_idx] + this->m_bytes[start_idx + 1];
}

u1 JavaClass::read_u1(size_t idx) {
	return this->m_bytes[idx];
}
