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

void CodeAttribute::parse(std::unique_ptr<ByteReader>& reader) {
	this->m_max_stack = reader->read_u2();
	this->m_max_locals = reader->read_u2();

	const auto code_length = reader->read_u4();
	std::vector<u1> code{};

	for (size_t i = 0; i < code_length; i++) {
		code.push_back(reader->read_u1());
	}

	const auto exception_table_length = reader->read_u2();

	for (auto i = 0; i < exception_table_length; i++) {
		const auto start_pc = reader->read_u2();
		const auto end_pc = reader->read_u2();
		const auto handler_pc = reader->read_u2();
		const auto catch_type = reader->read_u2();

		this->m_exception_table.push_back(ExceptionTableEntry{ start_pc, end_pc, handler_pc, catch_type });
	}

	const auto attributes_count = reader->read_u2();

	// no parsing (for now)
	for (auto i = 0; i < attributes_count; i++) {
		const auto attribute_name_index = reader->read_u2();
		const auto attribute_length = reader->read_u4();

		std::vector<u1> attribute_info;

		for (size_t k = 0; k < attribute_length; k++) {
			attribute_info.push_back(reader->read_u1());
		}

		auto annotation_reader = std::make_unique<ByteReader>(this->m_java_class, attribute_info);

		const auto attribute = std::make_shared<BasicAttribute>(UnparsedJavaAttribute(this->m_java_class, attribute_name_index));
		std::get<UnparsedJavaAttribute>(*attribute).parse(annotation_reader);

		this->m_attributes.push_back(attribute);
	}

	this->parse_instructions(code);
}

std::vector<u1> CodeAttribute::get_bytes() {
	const auto writer = std::make_unique<ByteWriter>();

	writer->write_u2(this->m_max_stack);
	writer->write_u2(this->m_max_locals);

	std::vector<u1> code{};
	u4 current_address = 0;

	for (const auto& [instruction, bytes] : this->m_instructions) {
		if (instruction != BytecodeInstruction::LABEL) {
			auto code_reader = std::make_unique<ByteReader>(this->m_java_class, bytes);
			auto code_writer = std::make_unique<ByteWriter>();

			const auto instruction_name = magic_enum::enum_name(instruction);

			code.push_back((u1)instruction);

			if (instruction_name.starts_with("IF") || instruction_name.starts_with("GOTO") || instruction_name.starts_with("JSR")) {
				u2 label_idx;
				bool is_wide = instruction_name.ends_with("_W");

				if (is_wide) {
					label_idx = (u2)code_reader->read_u4();;
				} else {
					label_idx = code_reader->read_u2();
				}

				const s4 jump_offset = this->m_label_to_address[label_idx] - current_address;

				if (is_wide) {
					code_writer->write_s4(jump_offset);
				} else {
					code_writer->write_s2((s2)jump_offset);
				}
			} else if (instruction == BytecodeInstruction::LOOKUPSWITCH || instruction == BytecodeInstruction::TABLESWITCH) {
				u1 pad_amount = 0;

				while (((current_address + 1) + pad_amount) % 4 != 0) {
					code_writer->write_u1(code_reader->read_u1());
					pad_amount++;
				}

				const auto default_label = code_reader->read_u4();
				code_writer->write_s4(this->m_label_to_address[default_label] - current_address);
				
				if (instruction == BytecodeInstruction::LOOKUPSWITCH) {
					const auto num_offsets = code_reader->read_u4();
					code_writer->write_u4(num_offsets);

					for (size_t j = 0; j < num_offsets; j++) {
						const auto match = code_reader->read_u4();
						const auto label = code_reader->read_u4();

						code_writer->write_u4(match);
						code_writer->write_s4(this->m_label_to_address[label] - current_address);

					}
				} else {
					s4 low = code_reader->read_s4();
					s4 high = code_reader->read_s4();

					code_writer->write_s4(low);
					code_writer->write_s4(high);

					for (size_t j = 0; j < (high - low + 1); j++) {
						code_writer->write_s4(this->m_label_to_address[code_reader->read_u4()] - current_address);
					}
				}
			} else {
				for (const auto& byte : bytes) {
					code_writer->write_u1(byte);
				}
			}

			current_address += 1 + (u4)code_writer->get_bytes().size();

			for (const auto& byte : code_writer->get_bytes()) {
				code.push_back(byte);
			}
		}
	}

	writer->write_u4((u4)code.size());
	
	for (const auto& byte : code) {
		writer->write_u1(byte);
	}

	writer->write_u2((u2)this->m_exception_table.size());

	for (const auto& entry : this->m_exception_table) {
		writer->write_u2(entry.m_start_pc);
		writer->write_u2(entry.m_end_pc);
		writer->write_u2(entry.m_handler_pc);
		writer->write_u2(entry.m_catch_type);
	}

	writer->write_u2((u2)this->m_attributes.size());

	for (const auto& attribute_ptr : this->m_attributes) {
		std::visit([this, &writer](auto& attribute) {
			auto bytes = attribute.get_bytes();

			writer->write_u2(attribute.get_name_index());
			writer->write_u4((u4)bytes.size());

			for (const auto& byte : bytes) {
				writer->write_u1(byte);
			}
		}, *attribute_ptr);
	}

	return writer->get_bytes();
}

std::vector<std::string> CodeAttribute::get_code_string() {
	std::vector<std::string> list{};
	u4 current_address = 0;

	for (const auto [instruction_type, instruction_bytes] : this->m_instructions) {
		std::stringstream ss;
		const auto instruction_name = magic_enum::enum_name(instruction_type);
		const auto reader = std::make_unique<ByteReader>(this->m_java_class, instruction_bytes);

		if (instruction_name.starts_with("IF") || instruction_name.starts_with("GOTO") || instruction_name.starts_with("JSR")) {
			u4 label_idx;

			if (instruction_name.ends_with("_W")) {
				label_idx = reader->read_u4();
				current_address += 4;
			} else {
				label_idx = reader->read_u2();
				current_address += 2;
			}

			ss << instruction_name << " " << this->m_label_to_name[label_idx];
		} else {
			if (instruction_bytes.size() == 2) {
				const auto idx = reader->read_u2();

				if (instruction_type == BytecodeInstruction::LABEL) {
					ss << this->m_label_to_name[idx] << ":";
				} else if (instruction_type == BytecodeInstruction::IINC) {
					ss << instruction_name << " " << ((idx >> 8) & 255) << " " << (idx & 255);
				} else {
					ss << instruction_name << " #" << idx << " // " << get_constant_pool_string_for_code(idx);
				}

				current_address += 2;
			} else if (instruction_bytes.size() == 1) {
				const auto idx = reader->read_u1();

				if (instruction_type == BytecodeInstruction::LDC) {
					ss << instruction_name << " " << this->get_constant_pool_string_for_code((u2)idx);
				} else {
					ss << instruction_name << " " << (u4)idx;
				}

				current_address += 1;
			} else {
				switch (instruction_type) {
					case BytecodeInstruction::INVOKEDYNAMIC:
					case BytecodeInstruction::INVOKEINTERFACE:
					case BytecodeInstruction::INVOKESPECIAL: {
						const auto idx = reader->read_u2();
						ss << instruction_name << " #" << idx << " // " << get_constant_pool_string_for_code(idx);
						current_address += 2;
						break;
					}
					case BytecodeInstruction::MULTIANEWARRAY: {
						const auto class_ref = reader->read_u2();
						const auto dimensions = reader->read_u1();
						ss << instruction_name << " #" << class_ref << " " << dimensions << " // " << get_constant_pool_string_for_code(class_ref);
						current_address += 3;
						break;
					}
					case BytecodeInstruction::TABLESWITCH:
					case BytecodeInstruction::LOOKUPSWITCH: {
						u1 pad_amount = 0;

						while (((current_address + 1) + pad_amount) % 4 != 0) {
							reader->read_u1(reader->read_u1());
							pad_amount++;
						}

						current_address += pad_amount;

						const auto default_label_name = this->m_label_to_name[reader->read_u4()];
						
						if (instruction_type == BytecodeInstruction::LOOKUPSWITCH) {
							ss << instruction_name << " mapping[";

							const auto num_offsets = reader->read_u4();
							current_address += 8;

							for (size_t j = 0; j < num_offsets; j++) {
								const auto match = reader->read_u4();
								const auto label = reader->read_u4();

								ss << match << "=" << this->m_label_to_name[label];

								if (j != num_offsets - 1) {
									ss << ", ";
								}

								current_address += 8;
							}

							ss << "] default[" << default_label_name << "]";
						} else {
							const auto low = reader->read_s4();
							const auto high = reader->read_s4();
							current_address += 8;

							ss << instruction_name << " range[" << low << ":" << high << "] labels[";

							for (size_t j = 0; j < (high - low + 1); j++) {
								ss << this->m_label_to_name[reader->read_u4()];

								if (j != (high - low)) {
									ss << ", ";
								}

								current_address += 4;
							}

							ss << "] default[" << default_label_name << "]";
						}

						break;
					}
					default:
						ss << instruction_name;
						break;
				}
			}
		}

		current_address++; // for the instruction

		if (!ss.str().empty()) {
			list.push_back(ss.str());
		}
	}

	return list;
}

// Credit: https://github.com/Col-E/Recaf/blob/56c3b78d07caebbb3aa07ccc0a1d91200749f098/src/main/java/me/coley/recaf/util/StringUtil.java#L56-L81
std::string CodeAttribute::get_label_name(u2 idx) {
	static const std::vector<char> letters{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };

	std::string str;
	int alphabetLength = (int)letters.size();
	int m = 8;
	char* chars = new char[m];
	int n = m - 1;

	while (idx > letters.size() - 1) {
		int k = abs(-(idx % alphabetLength));
		chars[n--] = letters[k];
		idx /= alphabetLength;
		idx -= 1;
	}

	chars[n] = letters[idx];

	for (int i = 0; i < m - n; i++) {
		str += chars[n + i];
	}

	return str;
}

u2 CodeAttribute::get_label_index(u4 find_address) {
	for (const auto& [label_key, address] : this->m_label_to_address) {
		if (find_address == address) {
			return label_key;
		}
	}

	return this->m_current_label_index++;
}

u2 CodeAttribute::get_or_create_label(u4 address) {
	const auto label_index = this->get_label_index(address);

	if (this->m_label_to_address.find(label_index) != this->m_label_to_address.end()) 
		return label_index;

	this->m_label_to_address[label_index] = address;
	this->m_label_to_name[label_index] = this->get_label_name(label_index);

	return label_index;
}

void CodeAttribute::parse_instructions(std::vector<u1> code) {
	auto reader = std::make_unique<ByteReader>(this->m_java_class, code);

	u4 current_address = 0;

	for (int i = 0; i < code.size(); i++) {
		auto writer = std::make_unique<ByteWriter>();

		const auto instruction = (BytecodeInstruction)reader->read_u1();
		const auto instruction_name = magic_enum::enum_name(instruction);
		const auto is_two_byte_arg_instruction = std::find(TWO_BYTE_ARG_INSTRUCTIONS.begin(), TWO_BYTE_ARG_INSTRUCTIONS.end(), instruction) != TWO_BYTE_ARG_INSTRUCTIONS.end();

		if (instruction_name.starts_with("IF") || instruction_name.starts_with("GOTO") || instruction_name.starts_with("JSR")) {
			bool is_wide = instruction_name.ends_with("_W");
			s4 offset;

			if (is_wide) {
				offset = reader->read_u4();
			} else {
				offset = (s4)reader->read_s2();
			}

			const auto label_index = this->get_or_create_label(current_address + offset);

			if (is_wide) {
				auto wide_label_index = (u4)label_index;
				writer->write_u4(wide_label_index);
				i += 4;
			} else {
				writer->write_u2(label_index);
				i += 2;
			}
		} else {
			if (std::find(TWO_BYTE_ARG_INSTRUCTIONS.begin(), TWO_BYTE_ARG_INSTRUCTIONS.end(), instruction) != TWO_BYTE_ARG_INSTRUCTIONS.end()) {
				writer->write_u2(reader->read_u2());
				i += 2;
			} else if (std::find(ONE_BYTE_ARG_INSTRUCTIONS.begin(), ONE_BYTE_ARG_INSTRUCTIONS.end(), instruction) != ONE_BYTE_ARG_INSTRUCTIONS.end()) {
				writer->write_u1(reader->read_u1());
				i += 1;
			} else {
				switch (instruction) {
					case BytecodeInstruction::INVOKEDYNAMIC:
					case BytecodeInstruction::INVOKEINTERFACE:
					case BytecodeInstruction::INVOKESPECIAL:
					case BytecodeInstruction::MULTIANEWARRAY: {
						writer->write_u1(reader->read_u1());
						writer->write_u1(reader->read_u1());
						writer->write_u1(reader->read_u1());

						i += 3;
						break;
					}
					case BytecodeInstruction::TABLESWITCH:
					case BytecodeInstruction::LOOKUPSWITCH: {
						// between 0-3, to align default byte 1 at a multiple of 4 bytes
						u1 pad_amount = 0;

						while (((current_address + 1) + pad_amount) % 4 != 0) {
							writer->write_u1(reader->read_u1());
							pad_amount++;
						}

						i += pad_amount;

						// default label
						writer->write_u4(this->get_or_create_label(current_address + reader->read_s4()));
						i += 4;

						if (instruction == BytecodeInstruction::LOOKUPSWITCH) {
							s4 num_pairs = reader->read_s4();
							writer->write_s4(num_pairs);

							i += 4;

							for (size_t j = 0; j < num_pairs; j++) {
								auto match = reader->read_u4();
								auto offset = reader->read_s4();

								writer->write_u4(match);
								writer->write_u4(this->get_or_create_label(current_address + offset));

								i += 8;
							}
						} else {
							s4 low = reader->read_s4();
							s4 high = reader->read_s4();
						
							writer->write_s4(low);
							writer->write_s4(high);

							i += 8;
						
							for (size_t j = 0; j < (high - low + 1); j++) {
								writer->write_u4(this->get_or_create_label(current_address + reader->read_s4()));
								i += 4;
							}
						}

						break;
					}
				}
			}
		}

		if (instruction != BytecodeInstruction::LABEL) {
			current_address += 1 + (u4)writer->get_bytes().size();
		}

		this->m_instructions.push_back(std::make_pair(instruction, writer->get_bytes()));
	}

	// add exception labels
	// idx starts at 1
	// EX_START_IDX
	// EX_END_IDX
	// EX_HANDLER_IDX
	for (int i = 0; i < this->m_exception_table.size(); i++) {
		const auto entry = this->m_exception_table.at(i);
		
		const auto start_label_index = get_label_index(entry.m_start_pc);

		if (!this->m_label_to_address.contains(start_label_index)) {
			this->m_label_to_address[start_label_index] = entry.m_start_pc;
			this->m_label_to_name[start_label_index] = ("EX_START_" + std::to_string(i + 1));
		}

		if (entry.m_end_pc != entry.m_handler_pc) {
			const auto end_label_index = get_label_index(entry.m_end_pc);

			if (!this->m_label_to_address.contains(end_label_index)) {
				this->m_label_to_address[end_label_index] = entry.m_end_pc;
				this->m_label_to_name[end_label_index] = ("EX_END_" + std::to_string(i + 1));
			}
		}

		const auto handler_label_index = get_label_index(entry.m_handler_pc);

		if (!this->m_label_to_address.contains(handler_label_index)) {
			this->m_label_to_address[handler_label_index] = entry.m_handler_pc;
			this->m_label_to_name[handler_label_index] = ("EX_HANDLER_" + std::to_string(i + 1));
		}
	}

	// second pass for adding label pseudo-instructions
	current_address = 0;

	for (auto i = 0; i < this->m_instructions.size(); i++) {
		const auto instruction = this->m_instructions[i];

		for (const auto& [label_key, address] : this->m_label_to_address) {
			if (current_address == address) {
				const std::vector<u1> label_bytes = { (u1)((label_key >> 8) & 255), (u1)(label_key & 255) };
				const auto label_instruction = std::make_pair(BytecodeInstruction::LABEL, label_bytes);
				this->m_instructions.insert(this->m_instructions.begin() + i, label_instruction);
				i++;
			}
		}

		if (instruction.first != BytecodeInstruction::LABEL) {
			current_address += 1 + (u4)instruction.second.size();
		}
	}
}

std::string CodeAttribute::get_constant_pool_string_for_code(u2 idx) {
	auto constant_pool = this->m_java_class->get_constant_pool();
	const auto entry = constant_pool->get_entry(idx);
	const auto type = (ConstantPoolType)entry->m_tag;

	switch (type) {
		case ConstantPoolType::CONSTANT_String:
		case ConstantPoolType::CONSTANT_Utf8:
			return "\"" + constant_pool->get_string(idx) + "\"";
		case ConstantPoolType::CONSTANT_Fieldref: {
			const auto name_and_type = constant_pool->get_name_and_type((entry->m_info[2] << 8) | entry->m_info[3]);
			return constant_pool->get_string((entry->m_info[0] << 8) | entry->m_info[1]) + "." + name_and_type.first + " " + name_and_type.second;
		}
		case ConstantPoolType::CONSTANT_InterfaceMethodref:
		case ConstantPoolType::CONSTANT_Methodref: {
			const auto name_and_type = constant_pool->get_name_and_type((entry->m_info[2] << 8) | entry->m_info[3]);
			return constant_pool->get_string((entry->m_info[0] << 8) | entry->m_info[1]) + "." + name_and_type.first + name_and_type.second;
		}
		case ConstantPoolType::CONSTANT_Integer:
			return std::to_string(constant_pool->get_integer(idx));
		case ConstantPoolType::CONSTANT_Float:
			return std::to_string(constant_pool->get_float(idx)) + "F";
		case ConstantPoolType::CONSTANT_Double:
			return std::to_string(constant_pool->get_double(idx)) + "D";
		case ConstantPoolType::CONSTANT_Long:
			return std::to_string(constant_pool->get_long(idx)) + "L";
		default:
			return constant_pool->get_string(idx);
	}
}
