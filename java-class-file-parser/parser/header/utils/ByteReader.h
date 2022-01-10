#pragma once
#include "../defines.h"
#include "../types/JavaType.h"

#include <variant>
#include <vector>

class JavaClass;

class ByteReader : public JavaType {
public:
	ByteReader(JavaClass* java_class, std::vector<u1> bytes) : JavaType(java_class), m_bytes(bytes) {
		this->m_current_byte_index = 0;
	};

	// methods for reading bytes from the vector
	u4 read_u4() {
		const auto ret = this->read_u4(this->m_current_byte_index);
		this->m_current_byte_index += 4;
		return ret;
	}

	u2 read_u2() {
		const auto ret = this->read_u2(this->m_current_byte_index);
		this->m_current_byte_index += 2;
		return ret;
	}

	u1 read_u1() {
		const auto ret = this->read_u1(this->m_current_byte_index);
		this->m_current_byte_index += 1;
		return ret;
	}

	u4 read_u4(size_t start_idx) {
		return this->m_bytes[start_idx + 3] | (this->m_bytes[start_idx + 2] << 8) | (this->m_bytes[start_idx + 1] << 16) | (this->m_bytes[start_idx] << 24);
	}

	u2 read_u2(size_t start_idx) {
		return this->m_bytes[start_idx + 1] | (this->m_bytes[start_idx] << 8);
	}

	u1 read_u1(size_t idx) {
		return this->m_bytes[idx];
	}

	auto get_current_byte_index() {
		return this->m_current_byte_index;
	}

	void set_current_byte_index(auto index) {
		this->m_current_byte_index = index;
	}

	const auto get_bytes() {
		return this->m_bytes;
	}

protected:
	// bytes
	size_t m_current_byte_index;
	std::vector<u1> m_bytes;
};