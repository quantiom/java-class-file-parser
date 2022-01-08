#include "../../header/attributes/CodeAttribute.h"

void CodeAttribute::parse() {
	this->m_max_stack = this->read_u2();
	this->m_max_locals = this->read_u2();

	const auto code_length = this->read_u4();

	for (size_t i = 0; i < code_length; i++) {
		this->m_code.push_back(this->read_u1());
	}

	std::cout << "Code: ";

	for (const auto& code : this->m_code) {
		std::cout << std::hex << "0x" << (int)code << " ";
	}

	std::cout << std::endl << "Max stack: " << this->m_max_stack << " | " << "Max locals: " << this->m_max_locals << "\n";

	const auto exception_table_length = this->read_u2();

	for (auto i = 0; i < exception_table_length; i++) {
		const auto start_pc = this->read_u2();
		const auto end_pc = this->read_u2();
		const auto handler_pc = this->read_u2();
		const auto catch_type = this->read_u2();

		this->m_exception_table.push_back(ExceptionTableEntry{ start_pc, end_pc, handler_pc, catch_type });
	}

	const auto attributes_count = this->read_u2();

	// no parsing (for now)
	for (auto i = 0; i < attributes_count; i++) {
		const auto attribute_name_index = this->read_u2();
		const auto attribute_length = this->read_u4();

		std::vector<u1> attribute_info;

		for (auto k = 0; k < attribute_length; k++) {
			attribute_info.push_back(this->read_u1());
		}

		const auto attribute = std::make_shared<BasicAttribute>(JavaAttribute(this->m_java_class, attribute_name_index, attribute_info));

		this->m_attributes.push_back(attribute);
	}
}

// TODO: write back to byte writer
std::vector<u1> CodeAttribute::get_bytes() {
	return this->m_info;
}
