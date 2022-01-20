#include "../../header/utils/ConstantPool.h"
#include "../../header/types/JavaClass.h"

void ConstantPool::parse(JavaClass* java_class) {
	const auto constant_pool_count = java_class->read_u2();

	for (int i = 1; i < constant_pool_count; i++) {
		const auto tag = java_class->read_u1();
		std::optional<std::vector<u1>> byte_info;

		switch ((ConstantPoolType)tag) {
		case ConstantPoolType::CONSTANT_Fieldref:
		case ConstantPoolType::CONSTANT_InterfaceMethodref:
		case ConstantPoolType::CONSTANT_InvokeDynamic:
		case ConstantPoolType::CONSTANT_NameAndType:
		case ConstantPoolType::CONSTANT_Methodref: {
			const auto class_index = java_class->read_u2();
			const auto name_and_type_index = java_class->read_u2();

			std::cout << i << ": Reference" << "\n";

			byte_info = { (u1)(class_index >> 8), (u1)class_index, (u1)(name_and_type_index >> 8), (u1)name_and_type_index };
		} break;
		case ConstantPoolType::CONSTANT_MethodHandle: {
			const auto reference_kind = java_class->read_u1();
			const auto reference_index = java_class->read_u2();

			std::cout << i << ": Method Handle" << "\n";

			byte_info = { reference_kind, (u1)(reference_index >> 8), (u1)reference_index };
		} break;
		case ConstantPoolType::CONSTANT_MethodType:
		case ConstantPoolType::CONSTANT_Class:
		case ConstantPoolType::CONSTANT_String: {
			const auto name_index = java_class->read_u2();

			std::cout << i << ": Class, String, or MethodType" << "\n";

			byte_info = { (u1)(name_index >> 8), (u1)name_index };
		} break;
		case ConstantPoolType::CONSTANT_Integer:
		case ConstantPoolType::CONSTANT_Float: {
			const auto value = java_class->read_u4();
			const auto bytes = (u1*)&value;

			std::cout << i << ": Int/Float" << "\n";

			byte_info = { bytes[3], bytes[2], bytes[1], bytes[0] };
		} break;
		case ConstantPoolType::CONSTANT_Long:
		case ConstantPoolType::CONSTANT_Double: {
			const auto high_value = java_class->read_u4();
			const auto low_value = java_class->read_u4();

			const auto high_bytes = (u1*)&high_value;
			const auto low_bytes = (u1*)&low_value;

			std::cout << i << ": Long/Double" << "\n";

			byte_info = { high_bytes[3], high_bytes[2], high_bytes[1], high_bytes[0], low_bytes[3], low_bytes[2], low_bytes[1], low_bytes[0] };

			i += 1;
		} break;
		case ConstantPoolType::CONSTANT_Utf8: {
			const auto length = java_class->read_u2();

			std::cout << i << ": UTF8" << "\n";

			byte_info = { (u1)(length >> 8), (u1)length };

			for (int j = 0; j < length; j++) {
				byte_info.value().push_back(java_class->read_u1());
			}
		} break;
		}

		if (byte_info.has_value()) {
			this->add_entry(ConstantPoolEntryInfo{ tag, byte_info.value() });

			if ((ConstantPoolType)tag == ConstantPoolType::CONSTANT_Long || (ConstantPoolType)tag == ConstantPoolType::CONSTANT_Double) {
				this->add_entry(ConstantPoolEntryInfo{ (u1)0xFF, {} });
			}
		} else {
			throw std::runtime_error("Error parsing constant pool.");
		}
	}
}

void ConstantPool::add_entry(ConstantPoolEntryInfo entry) {
	this->m_entries.push_back(entry);
}

std::optional<ConstantPoolEntryInfo> ConstantPool::get_entry(size_t idx) {
	if (idx > 0 && idx < this->m_entries.size() + 1) {
		return this->m_entries.at(idx - 1);
	}

	return {};
}

std::string ConstantPool::get_string(size_t idx) {
	const auto entry = this->get_entry(idx);

	/*if (!entry.has_value() || (
		(ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Utf8 
		&& (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Class
		&& (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Fieldref
		&& (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_String)) {
		throw std::invalid_argument("Constant pool index is invalid or not a string.");
	}*/

	const auto type = (ConstantPoolType)entry->m_tag;
	const auto bytes = entry->m_info;

	if (type == ConstantPoolType::CONSTANT_Utf8) {
		std::string str{};
		const auto str_length = (u2)((bytes[0] << 8) | bytes[1]);

		for (auto i = 0; i < str_length; i++) {
			str += bytes[2 + i];
		}

		return str;
	} else if (type == ConstantPoolType::CONSTANT_String || type == ConstantPoolType::CONSTANT_Class) {
		return this->get_string((bytes[0] << 8) | bytes[1]);
	} else if (type == ConstantPoolType::CONSTANT_Fieldref || type == ConstantPoolType::CONSTANT_InterfaceMethodref || type == ConstantPoolType::CONSTANT_InvokeDynamic || type == ConstantPoolType::CONSTANT_NameAndType || type == ConstantPoolType::CONSTANT_Methodref) {
		return this->get_string((bytes[0] << 8) | bytes[1]) + ":" + this->get_string((bytes[2] << 8) | bytes[3]);
	}

	return "";
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

s4 ConstantPool::get_integer(size_t idx) {
	auto entry = this->get_entry(idx);

	if (!entry.has_value() || (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Integer) {
		throw std::invalid_argument("Constant pool index is invalid or not an integer.");
	}

	const auto bytes = entry->m_info;
	const std::vector<u1> backwards = { bytes[3], bytes[2], bytes[1], bytes[0] };

	return *(s4*)backwards.data();
}

float ConstantPool::get_float(size_t idx) {
	auto entry = this->get_entry(idx);

	if (!entry.has_value() || (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Float) {
		throw std::invalid_argument("Constant pool index is invalid or not a float.");
	}

	const auto bytes = entry->m_info;
	const std::vector<u1> backwards = { bytes[3], bytes[2], bytes[1], bytes[0] };

	return *(float*)backwards.data();
}

double ConstantPool::get_double(size_t idx) {
	auto entry = this->get_entry(idx);

	if (!entry.has_value() || (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Double) {
		throw std::invalid_argument("Constant pool index is invalid or not a double.");
	}

	const auto bytes = entry->m_info;
	const std::vector<u1> reordered = { bytes[7], bytes[6], bytes[5], bytes[4], bytes[3], bytes[2], bytes[1], bytes[0] };

	return *(double*)reordered.data();
}

long long ConstantPool::get_long(size_t idx) {
	auto entry = this->get_entry(idx);

	if (!entry.has_value() || (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Long) {
		throw std::invalid_argument("Constant pool index is invalid or not a long.");
	}

	const auto bytes = entry->m_info;
	const std::vector<u1> reordered = { bytes[7], bytes[6], bytes[5], bytes[4], bytes[3], bytes[2], bytes[1], bytes[0] };

	return *(long long*)reordered.data();
}

u2 ConstantPool::get_class(size_t idx) {
	auto entry = this->get_entry(idx);

	if (!entry.has_value() || (ConstantPoolType)entry->m_tag != ConstantPoolType::CONSTANT_Class) {
		throw std::invalid_argument("Constant pool index is invalid or not a Class.");
	}

	const auto bytes = entry->m_info;
	return (bytes[0] << 8) | bytes[1]; // name index
}

u2 ConstantPool::get_or_add_utf8(const std::string& str) {
	if (this->m_cached_strings.contains(str)) {
		return this->m_cached_strings.at(str);
	}

	for (auto i = 1; i < this->m_entries.size() + 1; i++) {
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

	this->m_entries.push_back(ConstantPoolEntryInfo{ (int)ConstantPoolType::CONSTANT_Utf8, info });

	const auto new_idx = (u2)this->m_entries.size();
	this->m_cached_strings[str] = new_idx;

	return new_idx;
}

u2 ConstantPool::get_or_add_class(const std::string& str) {
	if (this->m_cached_classes.contains(str)) {
		return this->m_cached_classes.at(str);
	}

	for (auto i = 1; i < this->m_entries.size() + 1; i++) {
		const auto entry = this->get_entry(i);

		if ((ConstantPoolType)entry->m_tag == ConstantPoolType::CONSTANT_Class) {
			if (this->get_string(this->get_class(i)) == str) {
				this->m_cached_classes[str] = i;
				return i;
			}
		}
	}

	const auto name_index = this->get_or_add_utf8(str);
	std::vector<u1> info = { (u1)((name_index >> 8) & 255), (u1)(name_index & 255) };

	this->m_entries.push_back(ConstantPoolEntryInfo{ (int)ConstantPoolType::CONSTANT_Class, info });

	const auto new_idx = (u2)this->m_entries.size();
	this->m_cached_classes[str] = new_idx;

	return new_idx;
}

u2 ConstantPool::get_or_add_integer(s4 integer) {
	if (this->m_cached_integers.contains(integer)) {
		return this->m_cached_integers.at(integer);
	}

	for (auto i = 1; i < this->m_entries.size() + 1; i++) {
		const auto entry = this->get_entry(i);

		if ((ConstantPoolType)entry->m_tag == ConstantPoolType::CONSTANT_Integer) {
			if (this->get_integer(i) == integer) {
				this->m_cached_integers[integer] = i;
				return i;
			}
		}
	}

	std::vector<u1> info = { (u1)((integer >> 24) & 255), (u1)((integer >> 16) & 255), (u1)((integer >> 8) & 255), (u1)(integer & 255) };

	this->m_entries.push_back(ConstantPoolEntryInfo{ (int)ConstantPoolType::CONSTANT_Integer, info });

	const auto new_idx = (u2)this->m_entries.size();
	this->m_cached_integers[integer] = new_idx;

	return new_idx;
}

std::vector<ConstantPoolEntryInfo> ConstantPool::get_entries() {
	return this->m_entries;
}

size_t ConstantPool::get_size() {
	return this->m_entries.size();
}
