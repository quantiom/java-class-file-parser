#pragma once
#include <array>

namespace hash {
	class StringWrapper {
	public:
		template <typename T>
		constexpr StringWrapper(T&& str) : m_str{ str }, m_size{ 0 } {
			if constexpr (std::is_array_v<std::remove_reference_t<T>>) {
				this->m_size = sizeof(str) - 1;
			} else {
				this->m_size = strlen(str);
			}
		}

		template <typename T>
		constexpr StringWrapper(T&& str, const size_t size) : m_str{ str }, m_size{ size } {}

		// this is dangerous
		StringWrapper(const std::string& str) : m_str{ str.c_str() }, m_size{ str.size() } {}

		// getters
		constexpr const char* string() const { return this->m_str; }
		constexpr size_t size() const { return this->m_size; }

	private:
		const char* const m_str;
		size_t m_size;
	};

	namespace impl {
		// fnv1 offset
		template <typename T>
		constexpr auto fnv1_offset() {
			if constexpr (sizeof(T) == 4) {
				return 2166136261ui32;
			} else if constexpr (sizeof(T) == 8) {
				return 14695981039346656037ui64;
			} else {
				static_assert(false, "Type not supported");
			}
		}

		// fnv1 prime
		template <typename T>
		constexpr auto fnv1_prime() {
			if constexpr (sizeof(T) == 4) {
				return 16777619ui32;
			} else if constexpr (sizeof(T) == 8) {
				return 1099511628211ui64;
			} else {
				static_assert(false, "Type not supported");
			}
		}
	} // namespace impl

	template <typename T> constexpr auto fnv1_offset_v = impl::fnv1_offset<T>();
	template <typename T> constexpr auto fnv1_prime_v = impl::fnv1_prime<T>();

	// compile-time FNV-1 class
	template <typename T = size_t>
	class Fnv1 {
	public:
		static_assert(std::is_integral_v<T>, "T must be an integral type");

		constexpr Fnv1() noexcept = default;

		// construct from a string
		template <size_t Size>
		constexpr Fnv1(const char(&str)[Size]) noexcept
			: m_value{ this->construct_hash(str) } {
		}
		constexpr Fnv1(const StringWrapper str) noexcept
			: m_value{ this->construct_hash(str) } {
		}

		// get the hash
		constexpr operator T() const noexcept { return this->m_value; }
		constexpr T operator()() const noexcept { return this->m_value; }

	private:
		// make the hash
		static constexpr T construct_hash(const StringWrapper str) {
			auto hash = fnv1_offset_v<T>;
			for (size_t i{ 0 }; i < str.size(); ++i)
				hash = (hash * fnv1_prime_v<T>) ^ str.string()[i];
			return hash;
		}

	private:
		const T m_value;
	};

	// compile-time FNV-1a class
	template <typename T = size_t>
	class Fnv1a {
	public:
		static_assert(std::is_integral_v<T>, "T must be an integral type");

		constexpr Fnv1a() noexcept = default;

		// construct from a string
		template <size_t Size>
		constexpr Fnv1a(const char(&str)[Size]) noexcept
			: m_value{ this->construct_hash(str) } {
		}
		constexpr Fnv1a(const StringWrapper str) noexcept
			: m_value{ this->construct_hash(str) } {
		}

		// get the hash
		constexpr operator T() const noexcept { return this->m_value; }
		constexpr T operator()() const noexcept { return this->m_value; }

	private:
		// make the hash
		static constexpr T construct_hash(const StringWrapper str) {
			auto hash = fnv1_offset_v<T>;
			for (size_t i{ 0 }; i < str.size(); ++i)
				hash = (hash ^ str.string()[i]) * fnv1_prime_v<T>;
			return hash;
		}

	private:
		const T m_value;
	};
} // namespace hash