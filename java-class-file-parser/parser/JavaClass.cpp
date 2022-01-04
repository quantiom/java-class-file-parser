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
			case ConstantPoolType::CONSTANT_InvokeDynamic:
			case ConstantPoolType::CONSTANT_NameAndType:
			case ConstantPoolType::CONSTANT_Methodref: {
				const auto class_index = this->read_u2();
				const auto name_and_type_index = this->read_u2();
				
				byte_info = { (u1)(class_index >> 8), (u1)class_index, (u1)(name_and_type_index >> 8), (u1)name_and_type_index };
			} break;
			case ConstantPoolType::CONSTANT_MethodHandle: {
				const auto reference_kind = this->read_u1();
				const auto reference_index = this->read_u2();

				byte_info = { reference_kind, (u1)(reference_index >> 8), (u1)reference_index };
			} break;
			case ConstantPoolType::CONSTANT_MethodType:
			case ConstantPoolType::CONSTANT_Class:
			case ConstantPoolType::CONSTANT_String: {
				const auto name_index = this->read_u2();

				byte_info = { (u1)(name_index >> 8), (u1)name_index };
			} break;
			case ConstantPoolType::CONSTANT_Integer:
			case ConstantPoolType::CONSTANT_Float: {
				const auto value = this->read_u4();
				const auto bytes = (u1*)&value;

				byte_info = { bytes[0], bytes[1], bytes[2], bytes[3] };
			} break;
			case ConstantPoolType::CONSTANT_Long:
			case ConstantPoolType::CONSTANT_Double: {
				const auto low_value = this->read_u4();
				const auto high_value = this->read_u4();

				const auto low_bytes = (u1*)&low_value;
				const auto high_bytes = (u1*)&high_value;

				byte_info = { low_bytes[0], low_bytes[1], low_bytes[2], low_bytes[3], high_bytes[0], high_bytes[1], high_bytes[2], high_bytes[3] };
			} break;
			case ConstantPoolType::CONSTANT_Utf8: {
				const auto length = this->read_u2();
				
				byte_info = { (u1)(length >> 8), (u1)length };

				for (int j = 0; j < length; j++) {
					byte_info.value().push_back(this->read_u1());
				}
			} break;
		}

		if (byte_info.has_value()) {
			this->m_constant_pool.add_constant(ConstantPoolEntryInfo{ tag, byte_info.value() });
		} else {
			throw std::runtime_error("Error parsing constant pool.");
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
