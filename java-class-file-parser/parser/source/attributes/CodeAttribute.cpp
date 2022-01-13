#include "../../header/attributes/CodeAttribute.h"
#include "../../header/types/JavaClass.h"
#include "../../header/utils/magic_enum.h"

#include <sstream>

namespace {
	static std::vector<BytecodeInstruction> TWO_BYTE_ARG_INSTRUCTIONS{
		BytecodeInstruction::ANEWARRAY,
		BytecodeInstruction::CHECKCAST,
		BytecodeInstruction::GETFIELD,
		BytecodeInstruction::GETSTATIC,
		BytecodeInstruction::GOTO,
		BytecodeInstruction::IF_ACMPEQ,
		BytecodeInstruction::IF_ACMPNE,
		BytecodeInstruction::IF_ICMPEQ,
		BytecodeInstruction::IF_ICMPGE,
		BytecodeInstruction::IF_ICMPGT,
		BytecodeInstruction::IF_ICMPLE,
		BytecodeInstruction::IF_ICMPLT,
		BytecodeInstruction::IF_ICMPNE,
		BytecodeInstruction::IFEQ,
		BytecodeInstruction::IFGE,
		BytecodeInstruction::IFGT,
		BytecodeInstruction::IFLE,
		BytecodeInstruction::IFLT,
		BytecodeInstruction::IFNE,
		BytecodeInstruction::IFNONNULL,
		BytecodeInstruction::IFNULL,
		BytecodeInstruction::IINC,
		BytecodeInstruction::INSTANCEOF,
		BytecodeInstruction::INVOKESPECIAL,
		BytecodeInstruction::INVOKESTATIC,
		BytecodeInstruction::INVOKEVIRTUAL,
		BytecodeInstruction::JSR,
		BytecodeInstruction::LDC_W,
		BytecodeInstruction::LDC2_W,
		BytecodeInstruction::NEW,
		BytecodeInstruction::PUTFIELD,
		BytecodeInstruction::PUTSTATIC,
		BytecodeInstruction::SIPUSH,
	};

	static std::vector<BytecodeInstruction> ONE_BYTE_ARG_INSTRUCTIONS{
		BytecodeInstruction::ALOAD,
		BytecodeInstruction::ASTORE,
		BytecodeInstruction::BIPUSH,
		BytecodeInstruction::DLOAD,
		BytecodeInstruction::DSTORE,
		BytecodeInstruction::FLOAD,
		BytecodeInstruction::FSTORE,
		BytecodeInstruction::ILOAD,
		BytecodeInstruction::ISTORE,
		BytecodeInstruction::LDC,
		BytecodeInstruction::LLOAD,
		BytecodeInstruction::LSTORE,
		BytecodeInstruction::NEWARRAY,
		BytecodeInstruction::RET,
	};
} // namespace

void CodeAttribute::parse() {
	this->m_max_stack = this->read_u2();
	this->m_max_locals = this->read_u2();

	const auto code_length = this->read_u4();

	for (size_t i = 0; i < code_length; i++) {
		this->m_code.push_back(this->read_u1());
	}

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

		for (size_t k = 0; k < attribute_length; k++) {
			attribute_info.push_back(this->read_u1());
		}

		const auto attribute = std::make_shared<BasicAttribute>(JavaAttribute(this->m_java_class, attribute_name_index, attribute_info));

		this->m_attributes.push_back(attribute);
	}

	this->parse_instructions();
}

std::vector<u1> CodeAttribute::get_bytes() {
	return this->get_info();
}

std::vector<std::string> CodeAttribute::get_code_string() {
	std::vector<std::string> list{};

	auto reader = std::make_unique<ByteReader>(ByteReader(this->m_java_class, this->m_code));

	std::cout << "instructions: " << this->m_instructions.size() << "\n";

	for (int i = 0; i < this->m_code.size(); i++) {
		std::cout << "0x" << std::hex << (int)this->m_code.at(i) << " ";
	}

	std::cout << "\n";

	for (int i = 0; i < this->m_code.size(); i++) {
		std::stringstream ss;

		const auto instruction = (BytecodeInstruction)reader->read_u1();
		const auto instruction_name = magic_enum::enum_name(instruction);

		const auto is_two_byte_arg_instruction = std::find(TWO_BYTE_ARG_INSTRUCTIONS.begin(), TWO_BYTE_ARG_INSTRUCTIONS.end(), instruction) != TWO_BYTE_ARG_INSTRUCTIONS.end();

		if (std::find(TWO_BYTE_ARG_INSTRUCTIONS.begin(), TWO_BYTE_ARG_INSTRUCTIONS.end(), instruction) != TWO_BYTE_ARG_INSTRUCTIONS.end()) {
			// TODO: jumps and labels
			// I think it would be better to make a different method
			// that returns a list of (Instruction, std::vector<u1>) and then parse it here.
			// That way we could add a pseudo-instruction named something like "label"
			// and insert it farther in the list. It would be much easier to calculate the byte offset.

			const auto idx = reader->read_u2();
			ss << instruction_name << " #" << idx << " // " << get_constant_pool_string_for_code(idx);
			i += 2;
		} else if (std::find(ONE_BYTE_ARG_INSTRUCTIONS.begin(), ONE_BYTE_ARG_INSTRUCTIONS.end(), instruction) != ONE_BYTE_ARG_INSTRUCTIONS.end()) {
			const auto idx = reader->read_u1();

			if (instruction == BytecodeInstruction::LDC) {
				ss << instruction_name << " " << this->get_constant_pool_string_for_code((u2)idx);
			} else {
				ss << instruction_name << " " << (u4)idx;
			}

			i += 1;
		} else {
			switch (instruction) {
				case BytecodeInstruction::INVOKEDYNAMIC:
				case BytecodeInstruction::INVOKEINTERFACE:
				case BytecodeInstruction::INVOKESPECIAL: {
					const auto idx = reader->read_u2();
					ss << instruction_name << " #" << idx << " // " << get_constant_pool_string_for_code(idx);
					i += 4;
					break;
				}
				case BytecodeInstruction::JSR_W:
				case BytecodeInstruction::GOTO_W: {
					const auto offset = reader->read_u4();
					ss << instruction_name << " " << offset;
					i += 4;
					break;
				}
				case BytecodeInstruction::MULTIANEWARRAY: {
					const auto class_ref = reader->read_u2();
					const auto dimensions = reader->read_u1();
					ss << instruction_name << " #" << class_ref << " " << dimensions << " // " << get_constant_pool_string_for_code(class_ref);
					i += 3;
					break;
				}
				default:
					ss << instruction_name;
					break;
			}
		}

		if (!ss.str().empty()) {
			list.push_back(ss.str());
		}
	}

	return list;
}

void CodeAttribute::parse_instructions() {
	auto reader = std::make_unique<ByteReader>(ByteReader(this->m_java_class, this->m_code));

	for (int i = 0; i < this->m_code.size(); i++) {
		std::vector<u1> bytes;

		const auto instruction = (BytecodeInstruction)reader->read_u1();
		const auto is_two_byte_arg_instruction = std::find(TWO_BYTE_ARG_INSTRUCTIONS.begin(), TWO_BYTE_ARG_INSTRUCTIONS.end(), instruction) != TWO_BYTE_ARG_INSTRUCTIONS.end();

		if (std::find(TWO_BYTE_ARG_INSTRUCTIONS.begin(), TWO_BYTE_ARG_INSTRUCTIONS.end(), instruction) != TWO_BYTE_ARG_INSTRUCTIONS.end()) {
			bytes.push_back(reader->read_u1());
			bytes.push_back(reader->read_u1());
			i += 2;
		} else if (std::find(ONE_BYTE_ARG_INSTRUCTIONS.begin(), ONE_BYTE_ARG_INSTRUCTIONS.end(), instruction) != ONE_BYTE_ARG_INSTRUCTIONS.end()) {
			bytes.push_back(reader->read_u1());
			i += 1;
		} else {
			switch (instruction) {
				case BytecodeInstruction::INVOKEDYNAMIC:
				case BytecodeInstruction::INVOKEINTERFACE:
				case BytecodeInstruction::INVOKESPECIAL:
				case BytecodeInstruction::JSR_W:
				case BytecodeInstruction::GOTO_W: {
					for (auto i = 0; i < 4; i++) bytes.push_back(this->read_u1());
					i += 4;
					break;
				}
				case BytecodeInstruction::MULTIANEWARRAY: {
					bytes.push_back(reader->read_u1());
					bytes.push_back(reader->read_u1());
					bytes.push_back(reader->read_u1());
					i += 3;
					break;
				}
			}
		}

		this->m_instructions.push_back(std::make_pair(instruction, bytes));
	}

	auto actual_byte_index = 0;

	for (int i = 0; i < this->m_instructions.size(); i++) {
		const auto instruction = this->m_instructions.at(i);
		const auto instruction_name = magic_enum::enum_name(instruction.first);

		actual_byte_index += 1 + instruction.second.size();

		if (instruction_name.starts_with("IF") || instruction_name.starts_with("GOTO")) {
			int offset;

			if (instruction.second.size() == 4) {
				offset = (instruction.second[0] << 24) | (instruction.second[1] << 16) | (instruction.second[2] << 8) | instruction.second[3];
			} else {
				offset = (instruction.second[0] << 8) | instruction.second[1];
			}

			const auto label_index = this->m_current_label_index++;
			this->m_label_to_address[label_index] = actual_byte_index + offset;
		}
	}

	for (const auto& [k, v] : this->m_label_to_address) {
		std::cout << "label: " << k << " ; address: " << v << "\n";
	}

	// jump A
	// ...
	// A:
	// -> 10 is here ; LDC "AAA"

	// LDC "Frog"
	// INVOKEDYNAMIC BLABLA
	// POP
	// ICONST_0
	// IFLT 10 // jump 10 bytes
	// ICONST_1
	// POP
	// A: label a
	// IFLT 20 // jump 20 bytes
	// ICONST_0
	// POP
}

std::string CodeAttribute::get_constant_pool_string_for_code(u2 idx) {
	auto constant_pool = this->m_java_class->get_constant_pool();
	const auto entry = constant_pool.get_entry(idx);
	const auto type = (ConstantPoolType)entry->m_tag;

	switch (type) {
		case ConstantPoolType::CONSTANT_String:
		case ConstantPoolType::CONSTANT_Utf8:
			return "\"" + constant_pool.get_string(idx) + "\"";
		case ConstantPoolType::CONSTANT_Fieldref: {
			const auto name_and_type = constant_pool.get_name_and_type((entry->m_info[2] << 8) | entry->m_info[3]);
			return constant_pool.get_string((entry->m_info[0] << 8) | entry->m_info[1]) + "." + name_and_type.first + " " + name_and_type.second;
		}
		case ConstantPoolType::CONSTANT_InterfaceMethodref:
		case ConstantPoolType::CONSTANT_Methodref: {
			const auto name_and_type = constant_pool.get_name_and_type((entry->m_info[2] << 8) | entry->m_info[3]);
			return constant_pool.get_string((entry->m_info[0] << 8) | entry->m_info[1]) + "." + name_and_type.first + name_and_type.second;
		}
		case ConstantPoolType::CONSTANT_Integer:
			return std::to_string(constant_pool.get_integer(idx));
		case ConstantPoolType::CONSTANT_Float:
			return std::to_string(constant_pool.get_float(idx)) + "F";
		case ConstantPoolType::CONSTANT_Double:
			return std::to_string(constant_pool.get_double(idx)) + "D";
		case ConstantPoolType::CONSTANT_Long:
			return std::to_string(constant_pool.get_long(idx)) + "L";
		default:
			return constant_pool.get_string(idx);
	}
}
