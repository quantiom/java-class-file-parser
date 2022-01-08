#include <optional>
#include "../../header/types/JavaClass.h"
#include "../../header/utils/AttributeWriter.h"
#include "../../header/attributes/CodeAttribute.h"

JavaClass* JavaClass::parse() {
	if (this->read_u4() != 0xCAFEBABE) {
		throw std::invalid_argument("Invalid java class file. Does not have 0xCAFEBABE magic.");
	}

	this->m_minor_version = this->read_u2();
	this->m_major_version = this->read_u2();

	std::cout << "Constant Pool: " << std::endl;
	this->parse_constant_pool();

	this->m_access_flags = this->read_u2();

	this->m_this_class_idx = this->read_u2();
	this->m_super_class_idx = this->read_u2();

	this->parse_interfaces();
	this->parse_fields();
	this->parse_methods();
	this->parse_attributes();

	return this;
}

std::vector<u1> JavaClass::get_bytes() {
	// create new ByteWriter
	std::unique_ptr<AttributeWriter> writer(new AttributeWriter());

	// magic
	writer->write_u4(0xCAFEBABE);

	// version
	writer->write_u2(this->m_minor_version);
	writer->write_u2(this->m_major_version);

	// constant pool
	writer->write_u2((u2)(this->m_constant_pool.get_size() + 1));

	for (const auto& entry : this->m_constant_pool.get_entries()) {
		if (entry.m_tag == 0xFF) continue; // fake element to pad double/long

		writer->write_u1(entry.m_tag);

		for (auto i = 0; i < entry.m_info.size(); i++) {
			writer->write_u1(entry.m_info[i]);
		}
	}

	// access flags
	writer->write_u2(this->m_access_flags);

	// this & super class
	writer->write_u2(this->m_this_class_idx);
	writer->write_u2(this->m_super_class_idx);

	// interfaces
	writer->write_u2((u2)this->m_interfaces.size());

	for (const auto& interface : this->m_interfaces) {
		writer->write_u2(interface);
	}

	// fields
	writer->write_u2((u2)this->m_fields.size());

	for (const auto& field : this->m_fields) {
		writer->write_u2(field->get_access_flags());
		writer->write_u2(field->get_name_index());
		writer->write_u2(field->get_descriptor_index());
		writer->write_attributes(field->get_attributes());
	}

	// methods
	writer->write_u2((u2)this->m_methods.size());

	for (const auto& method : this->m_methods) {
		writer->write_u2(method->m_access_flags);
		writer->write_u2(method->m_name_index);
		writer->write_u2(method->m_descriptor_index);
		writer->write_attributes(method->m_attributes);
	}

	// class attributes
	writer->write_attributes(this->m_class_attributes);

	// return the written bytes
	return writer->m_bytes;
}

ConstantPool JavaClass::get_constant_pool() {
	return this->m_constant_pool;
}

std::vector<JavaField*> JavaClass::get_fields() {
	return this->m_fields;
}

std::vector<JavaMethod*> JavaClass::get_methods() {
	return this->m_methods;
}

std::vector<std::shared_ptr<ParsedAttribute>> JavaClass::get_class_attributes() {
	return this->m_class_attributes;
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
				
				std::cout << i << ": Reference" << "\n";
				
				byte_info = { (u1)(class_index >> 8), (u1)class_index, (u1)(name_and_type_index >> 8), (u1)name_and_type_index };
			} break;
			case ConstantPoolType::CONSTANT_MethodHandle: {
				const auto reference_kind = this->read_u1();
				const auto reference_index = this->read_u2();

				std::cout << i << ": Method Handle" << "\n";

				byte_info = { reference_kind, (u1)(reference_index >> 8), (u1)reference_index };
			} break;
			case ConstantPoolType::CONSTANT_MethodType:
			case ConstantPoolType::CONSTANT_Class:
			case ConstantPoolType::CONSTANT_String: {
				const auto name_index = this->read_u2();

				std::cout << i << ": Class, String, or MethodType" << "\n";

				byte_info = { (u1)(name_index >> 8), (u1)name_index };
			} break;
			case ConstantPoolType::CONSTANT_Integer:
			case ConstantPoolType::CONSTANT_Float: {
				const auto value = this->read_u4();
				const auto bytes = (u1*)&value;

				std::cout << i << ": Int/Float" << "\n";

				byte_info = { bytes[3], bytes[2], bytes[1], bytes[0] };
			} break;
			case ConstantPoolType::CONSTANT_Long:
			case ConstantPoolType::CONSTANT_Double: {
				const auto high_value = this->read_u4();
				const auto low_value = this->read_u4();

				const auto high_bytes = (u1*)&high_value;
				const auto low_bytes = (u1*)&low_value;

				std::cout << i << ": Long/Double" << "\n";

				byte_info = { high_bytes[3], high_bytes[2], high_bytes[1], high_bytes[0], low_bytes[3], low_bytes[2], low_bytes[1], low_bytes[0] };

				i += 1;
			} break;
			case ConstantPoolType::CONSTANT_Utf8: {
				const auto length = this->read_u2();

				std::cout << i << ": UTF8" << "\n";
				
				byte_info = { (u1)(length >> 8), (u1)length };

				for (int j = 0; j < length; j++) {
					byte_info.value().push_back(this->read_u1());
				}
			} break;
		}

		if (byte_info.has_value()) {
			this->m_constant_pool.add_constant(ConstantPoolEntryInfo{ tag, byte_info.value() });

			if ((ConstantPoolType)tag == ConstantPoolType::CONSTANT_Long || (ConstantPoolType)tag == ConstantPoolType::CONSTANT_Double) {
				this->m_constant_pool.add_constant(ConstantPoolEntryInfo{ (u1)0xFF, {} });
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

	for (auto i = 0; i < fields_count; i++) {
		const auto access_flags = this->read_u2();
		const auto name_index = this->read_u2();
		const auto descriptor_index = this->read_u2();

		const auto attribute_count = this->read_u2();

		std::vector<std::shared_ptr<ParsedAttribute>> attributes;

		for (auto j = 0; j < attribute_count; j++) {
			attributes.push_back(this->read_attribute());
		}

		this->m_fields.push_back(new JavaField(this, access_flags, name_index, descriptor_index, attributes));
	}
}

void JavaClass::parse_methods() {
	const auto methods_count = this->read_u2();

	for (auto i = 0; i < methods_count; i++) {
		const auto access_flags = this->read_u2();
		const auto name_index = this->read_u2();
		const auto descriptor_index = this->read_u2();

		const auto attribute_count = this->read_u2();

		std::vector<std::shared_ptr<ParsedAttribute>> attributes;

		for (auto j = 0; j < attribute_count; j++) {
			const auto attribute = this->read_attribute();
			attributes.push_back(attribute);

			if (this->get_constant_pool().get_string(name_index) == "main") {
				if (auto code_attribute = std::get_if<CodeAttribute>(&(*attribute)); code_attribute != nullptr) {
					std::cout << "max stack: " << code_attribute->get_max_stack() << "\n";
				}
			}
		}

		this->m_methods.push_back(new JavaMethod(this, access_flags, name_index, descriptor_index, attributes));
	}
}

void JavaClass::parse_attributes() {
	const auto attributes_count = this->read_u2();

	for (auto i = 0; i < attributes_count; i++) {
		this->m_class_attributes.push_back(this->read_attribute());
	}
}