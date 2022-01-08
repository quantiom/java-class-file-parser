#pragma once
#include "JavaAttribute.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.3

struct ExceptionTableEntry {
    u2 m_start_pc;
    u2 m_end_pc;
    u2 m_handler_pc;
    u2 m_catch_type;
};

// for attributes that can't have nested attributes
// this is needed, since ParsedAttribute can't be used
// because CodeAttribute needs to be in it
using BasicAttribute = std::variant<JavaAttribute>;

struct CodeAttribute : public JavaAttribute {
    // used in the variant
    CodeAttribute() : JavaAttribute() {};

	CodeAttribute(JavaClass* java_class, std::shared_ptr<JavaAttribute> attribute) : JavaAttribute(java_class, attribute) {};

    CodeAttribute(JavaClass* java_class, u2 name_index, std::vector<u1> info)
        : JavaAttribute(java_class, name_index, info) {};

	void parse();
	std::vector<u1> get_bytes();

    const auto get_max_stack() { return this->m_max_stack; }
    const auto get_max_locals() { return this->m_max_locals; }
    const auto get_code() { return this->m_code; };
    const auto get_exception_table() { return this->m_exception_table; }
    const auto get_attributes() { return this->m_attributes; }

private:
	u2 m_max_stack;
	u2 m_max_locals;

    std::vector<u1> m_code;

    std::vector<ExceptionTableEntry> m_exception_table;
  
    std::vector<std::shared_ptr<BasicAttribute>> m_attributes;
};