#include "../../header/utils/ConstantPool.h"

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

	const u2 class_index = (bytes[0] << 8) | bytes[1];
	const u2 name_and_type_index = (bytes[2] << 8) | bytes[3];

	return { class_index, name_and_type_index };
}

std::pair<std::string, std::string> ConstantPool::get_name_and_type(size_t idx) {
	auto entry = this->get_entry(idx);

	if (!entry.has_value() || (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_NameAndType) {
		throw std::invalid_argument("Constant pool index is invalid or not a NameAndType.");
	}

	const auto bytes = entry->m_info;

	const u2 name_index = (bytes[0] << 8) | bytes[1];
	const u2 descriptor_index = (bytes[2] << 8) | bytes[3];

	return std::make_pair<std::string, std::string>(this->get_string(name_index), this->get_string(descriptor_index));
}

u4 ConstantPool::get_integer(size_t idx) {
	auto entry = this->get_entry(idx);

	if (!entry.has_value() || (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Integer) {
		throw std::invalid_argument("Constant pool index is invalid or not an integer.");
	}

	return *(u4*)entry->m_info.data();
}

float ConstantPool::get_float(size_t idx) {
	auto entry = this->get_entry(idx);

	if (!entry.has_value() || (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Float) {
		throw std::invalid_argument("Constant pool index is invalid or not a float.");
	}

	return *(float*)entry->m_info.data();
}

// this is pretty bad

double ConstantPool::get_double(size_t idx) {
	auto entry = this->get_entry(idx);

	if (!entry.has_value() || (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Double) {
		throw std::invalid_argument("Constant pool index is invalid or not a double.");
	}

	const auto bytes = entry->m_info;

	const std::vector<u1> reordered = { bytes[4], bytes[5], bytes[6], bytes[7], bytes[0], bytes[1], bytes[2], bytes[3] };

	return *(double*)reordered.data();
}

long long ConstantPool::get_long(size_t idx) {
	auto entry = this->get_entry(idx);

	if (!entry.has_value() || (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Long) {
		throw std::invalid_argument("Constant pool index is invalid or not a long.");
	}

	const auto bytes = entry->m_info;

	const std::vector<u1> reordered = { bytes[4], bytes[5], bytes[6], bytes[7], bytes[0], bytes[1], bytes[2], bytes[3] };

	return *(long long*)reordered.data();
}

size_t ConstantPool::get_or_add_utf8(std::string str) {
	if (this->m_cached_strings.contains(str)) {
		return this->m_cached_strings.at(str);
	}

	// 1 to i + 1 because constant pool starts at 1
	for (auto i = 1; i < this->m_constant_pool.size() + 1; i++) {
		const auto entry = this->get_entry(i);

		if ((ConstantPoolType)entry->m_tag == ConstantPoolType::CONSTANT_Utf8) {
			if (this->get_string(i) == str) {
				this->m_cached_strings[str] = i;
				return i;
			}
		}
	}

	const auto size = str.size();
	std::vector<u1> info = { (u1)((size >> 8) & 255), (u1)(size & 255) };

	for (auto i = 0; i < size; i++) {
		info.push_back(str[i]);
	}

	this->m_constant_pool.push_back(ConstantPoolEntryInfo{ (int)ConstantPoolType::CONSTANT_Utf8, info });

	const auto new_idx = this->m_constant_pool.size();
	this->m_cached_strings[str] = new_idx;

	return new_idx;
}
