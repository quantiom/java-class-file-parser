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

	this->m_constant_pool = std::make_shared<ConstantPool>(ConstantPool());
	this->m_constant_pool->parse(this);

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
	writer->write_u2((u2)(this->m_constant_pool->get_size() + 1));

	for (const auto& entry : this->m_constant_pool->get_entries()) {
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
		writer->write_u2(method->get_access_flags());
		writer->write_u2(method->get_name_index());
		writer->write_u2(method->get_descriptor_index());
		writer->write_attributes(method->get_attributes());
	}

	// class attributes
	writer->write_attributes(this->m_attributes);

	// return the written bytes
	return writer->m_bytes;
}

std::shared_ptr<ConstantPool> JavaClass::get_constant_pool() {
	return this->m_constant_pool;
}

const std::vector<JavaField*> JavaClass::get_fields() {
	return this->m_fields;
}

const std::vector<JavaMethod*> JavaClass::get_methods() {
	return this->m_methods;
}

void JavaClass::set_minor_version(u2 new_minor_version) {
	this->m_minor_version = new_minor_version;
}

void JavaClass::set_major_version(u2 new_major_version) {
	this->m_major_version = new_major_version;
}

void JavaClass::set_access_flags(u2 new_access_flags) {
	this->m_access_flags = new_access_flags;
}

void JavaClass::set_super_class_idx(u2 new_class_idx) {
	this->m_super_class_idx = new_class_idx;
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
			attributes.push_back(this->read_attribute());
		}

		this->m_methods.push_back(new JavaMethod(this, access_flags, name_index, descriptor_index, attributes));
	}
}

void JavaClass::parse_attributes() {
	const auto attributes_count = this->read_u2();

	for (auto i = 0; i < attributes_count; i++) {
		this->m_attributes.push_back(this->read_attribute());
	}
}