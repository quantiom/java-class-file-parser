#pragma once
#include <iostream>
#include <vector>

#include "../defines.h"

// simple wrapper used to write parsed class files back to bytes

class ByteWriter {
public:
	// methods for writing bytes to the vector
	// (no clue if there's a better way to do this)

	void write_u1(u1 byte) {
		this->m_bytes.push_back(byte);
	}

	void write_u2(u2 bytes) {
		this->m_bytes.push_back((u1)((bytes >> 8) & 255));
		this->m_bytes.push_back((u1)(bytes & 255));
	}

	void write_u4(u4 bytes) {
		this->m_bytes.push_back((u1)((bytes >> 24) & 255));
		this->m_bytes.push_back((u1)((bytes >> 16) & 255));
		this->m_bytes.push_back((u1)((bytes >> 8) & 255));
		this->m_bytes.push_back((u1)(bytes & 255));
	}

	// bytes
	std::vector<u1> m_bytes;
};