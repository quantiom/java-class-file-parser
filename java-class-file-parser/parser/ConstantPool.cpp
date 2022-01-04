#include "ConstantPool.h"

void ConstantPool::add_constant(ConstantPoolEntryInfo entry) {
	this->m_constant_pool.push_back(entry);
}

std::optional<ConstantPoolEntryInfo> ConstantPool::get_entry(size_t idx) {
	if (idx > 0 && idx < this->m_constant_pool.size() + 1) {
		return this->m_constant_pool.at(idx - 1);
	}

	return {};
}

std::string ConstantPool::get_string(size_t idx) {
	const auto entry = this->get_entry(idx);

	if (!entry.has_value() || (
		(ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Utf8 
		&& (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Class
		&& (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_String)) {
		throw std::invalid_argument("Constant pool index is invalid or not a string.");
	}

	const auto type = (ConstantPoolType)entry->m_tag;
	const auto bytes = entry->m_info;

	if (type == ConstantPoolType::CONSTANT_Utf8) {
		std::string str{};
		const auto str_length = (u2)(bytes[0] + bytes[1]);

		for (auto i = 0; i < str_length; i++) {
			str += bytes[2 + i];
		}

		return str;
	} else if (type == ConstantPoolType::CONSTANT_String || type == ConstantPoolType::CONSTANT_Class) {
		return this->get_string(bytes[0] + bytes[1]);
	}
}

std::string ConstantPool::get_method_type(size_t idx) {
	return std::string();
}

ConstantPoolReference ConstantPool::get_reference(size_t idx) {
	const auto entry = this->get_entry(idx);

	if (!entry.has_value() || (
		(ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Fieldref
		&& (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_InterfaceMethodref
		&& (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Methodref
		&& (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_InvokeDynamic)) {
		throw std::invalid_argument("Constant pool index is invalid or not a reference.");
	}

	const auto bytes = entry->m_info;

	const u2 class_index = bytes[0] + bytes[1];

	const auto name_and_type_entry = this->get_entry(bytes[2] + bytes[3])->m_info;
	const auto name = this->get_string(name_and_type_entry[0] + name_and_type_entry[1]);
	const auto type = this->get_string(name_and_type_entry[2] + name_and_type_entry[3]);

	return { class_index, this->get_string(class_index), name, type };
}
