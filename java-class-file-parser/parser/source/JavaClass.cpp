#include <optional>
#include "../header/JavaClass.h"

void JavaClass::parse() {
	if (this->read_u4() != 0xCAFEBABE) {
		throw std::invalid_argument("Invalid java class file. Does not have 0xCAFEBABE magic.");
	}

	this->m_minor_version = this->read_u2();
	this->m_major_version = this->read_u2();

	this->parse_constant_pool();

	this->m_access_flags = this->read_u2();

	this->m_this_class_idx = this->read_u2();
	this->m_super_class_idx = this->read_u2();

	this->parse_interfaces();

	this->parse_fields();
}

// TODO: move to ConstantPool class
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

				i += 1;
			} break;
			case ConstantPoolType::CONSTANT_Long:
			case ConstantPoolType::CONSTANT_Double: {
				const auto high_value = this->read_u4();
				const auto low_value = this->read_u4();

				const auto high_bytes = (u1*)&high_value;
				const auto low_bytes = (u1*)&low_value;

				byte_info = { high_bytes[0], high_bytes[1], high_bytes[2], high_bytes[3], low_bytes[0], low_bytes[1], low_bytes[2], low_bytes[3] };
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

			if ((ConstantPoolType)tag == ConstantPoolType::CONSTANT_Long || (ConstantPoolType)tag == ConstantPoolType::CONSTANT_Double) {
				this->m_constant_pool.add_constant(ConstantPoolEntryInfo{});
			}
		} else {
			throw std::runtime_error("Error parsing constant pool.");
		}
	}
}

void JavaClass::parse_interfaces() {
	const auto interfaces_count = this->read_u2();

	for (auto i = 0; i < interfaces_count; i++) {
		this->m_interfaces.push_back(this->read_u2());
	}
}

void JavaClass::parse_fields() {
	const auto fields_count = this->read_u2();

	for (int i = 0; i < fields_count; i++) {
		const auto access_flags = this->read_u2();
		const auto name_index = this->read_u2();
		const auto descriptor_index = this->read_u2();

		const auto attribute_count = this->read_u2();

		std::cout << "Field " << this->m_constant_pool.get_string(name_index) << ", type " << this->m_constant_pool.get_string(descriptor_index) << '\n';

		for (int j = 0; j < attribute_count; j++) {
			const auto attribute = this->parse_attribute();

			std::cout << this->m_constant_pool.get_string(attribute.m_name_index) << "\n";
		}

		if (attribute_count != 0) break;
	}
}

Attribute JavaClass::parse_attribute() {
	const auto attribute_name_index = this->read_u2();
	const auto attribute_length = this->read_u4();

	std::vector<u1> attribute_info;

	for (int k = 0; k < attribute_length; k++) {
		attribute_info.push_back(this->read_u1());
	}

	return Attribute{ attribute_name_index, attribute_length, attribute_info };
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
	return this->m_bytes[start_idx + 3] | (this->m_bytes[start_idx + 2] << 8) | (this->m_bytes[start_idx + 1] << 16) | (this->m_bytes[start_idx] << 24);
}

u2 JavaClass::read_u2(size_t start_idx) {
	return this->m_bytes[start_idx + 1] | (this->m_bytes[start_idx] << 8);
}

u1 JavaClass::read_u1(size_t idx) {
	return this->m_bytes[idx];
}
