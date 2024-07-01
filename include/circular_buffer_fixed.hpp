//
// Created by Andrey Solovyev on 02.12.2023.
//

#pragma once

#include <iostream>
#include <vector>
#include <type_traits>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>

#ifdef __cpp_concepts
//#undef __cpp_concepts
#include <concepts>
#endif


namespace containers {

	namespace requirements {

		template <typename DataType>
		using IsDefaultConstructible = std::enable_if_t<std::is_default_constructible_v<DataType>, bool>;

	}//!namespace

	template <typename T>
	class CircularBufferFixed {
	public:
		using value_type = T;

#ifndef __cpp_concepts
		template <requirements::IsDefaultConstructible<T> = true>
#endif
		explicit CircularBufferFixed (int n)
#ifdef __cpp_concepts
		requires std::is_default_constructible_v<T>
#endif
				: cap 		{n}
				, sz        {0}
				, frontIdx 	{0}
				, backIdx 	{0}
		{
			if (n < 1) throw std::invalid_argument ("Can't create a fixed size circular buffer with size 0. Just why?..");
			data.resize(n, T{});
		}

		explicit CircularBufferFixed (int n, T defaultValue)
				: cap 		{n}
				, sz        {n}
				, frontIdx 	{0}
				, backIdx 	{n - 1}
		{
			if (n < 1) throw std::invalid_argument ("Can't create a fixed size circular buffer with size 0. Just why?..");
			data.resize(n, defaultValue);
		}

		void pushBack(T t) noexcept {
#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(likely)
			if (sz != 0u) [[likely]] {
				updatePushBack();
			}
			if (sz < cap) [[likely]] {
				++sz;
			}
	#else
			if (__builtin_expect(sz != 0u, 1)) {
				updatePushBack();
			}
			if (__builtin_expect(sz < cap), 1){
				++sz;
			}
#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
			data[backIdx] = std::move(t);
		}

		void pushFront(T t) noexcept {
#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(likely)
			if (sz != 0u) [[likely]] {
				updatePushFront();
			}
			if (sz < cap) [[likely]] {
				++sz;
			}
	#else
			if (__builtin_expect(sz != 0u, 1)) {
				updatePushFront();
			}
			if (__builtin_expect(sz < cap), 1) {
				++sz;
			}
#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
			data[frontIdx] = std::move(t);
		}

		T& popBack() noexcept {
			T& res = data[backIdx];
#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(likely)
			if (sz != 1 && frontIdx != backIdx) [[likely]] {
				updatePopBack();
			}
			if (sz > 0) [[likely]] {
				--sz;
			}
	#else
			if (__builtin_expect(sz != 1 && frontIdx != backIdx), 1) {
				updatePopBack();
			}
			if (__builtin_expect(sz > 0), 1) {
				--sz;
			}

	#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
			return res;
		}

		T& popFront() noexcept {
			T& res = data[frontIdx];
#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(likely)
			if (sz != 1 && frontIdx != backIdx) [[likely]] {
				updatePopFront();
			}
			if (sz > 0)  [[likely]] {
				--sz;
			}
	#else
			if (__builtin_expect(sz != 1 && frontIdx != backIdx), 1) {
				updatePopFront();
			}
			if (__builtin_expect(sz > 0), 1){
				--sz;
			}
	#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
			return res;
		}

		T& front() noexcept {
			return data[frontIdx];
		}

		T const& front() const {
			return data.at(frontIdx);
		}

		T& back() noexcept {
			return data[backIdx];
		}

		T const& back() const {
			return data.at(backIdx);
		}

		T& at(std::size_t idx) {
			auto idx_ = getIdx(idx);
			if (idx_ >= cap) {
				throw std::out_of_range("CircularBufferFixed is out of range with idx " + std::to_string(idx));
			}
			return data[idx_];
		}

		T const& at(std::size_t idx) const {
			auto idx_ = getIdx(idx);
			if (idx_ >= cap) {
				throw std::out_of_range("CircularBufferFixed is out of range with idx " + std::to_string(idx));
			}
			return data.at(idx_);
		}

		T& operator[](std::size_t idx) noexcept {
			auto idx_ = getIdx(idx);
			return data[idx_];
		}

		std::size_t capacity() const noexcept {
			return cap;
		}

		std::size_t size() const noexcept {
			return sz;
		}

		bool empty() const noexcept {
			return sz == 0u;
		}

	private:
		std::vector<T> data;
		std::int32_t const cap;
		std::int32_t sz;
		std::int32_t frontIdx, backIdx;

		inline void updatePushBack() noexcept {
			++backIdx;

#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(unlikely)
			if (backIdx == cap) [[unlikely]] {
				backIdx = 0;
			}
			if (backIdx == frontIdx) [[unlikely]] {
				++frontIdx;
				if (frontIdx == cap) [[unlikely]] {
					frontIdx = 0;
				}
			}
	#else
			if (__builtin_expect(backIdx == cap, 0)) {
				backIdx = 0;
			}
			if (__builtin_expect(backIdx == frontIdx, 0)) {
				++frontIdx;
				if (__builtin_expect(frontIdx == cap, 0)) {
					frontIdx = 0;
				}
			}
	#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
		}

		void updatePushFront() noexcept {
			--frontIdx;
#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(unlikely)
			if (frontIdx == -1) [[unlikely]] {
				frontIdx = cap - 1;
			}
			if(frontIdx == backIdx) [[unlikely]] {
				--backIdx;
				if (backIdx == -1) [[unlikely]] {
					backIdx = cap - 1;
				}
			}
	#else
			if (__builtin_expect(frontIdx == -1), 0){
				frontIdx = cap - 1;
			}
			if(__builtin_expect(frontIdx == backIdx), 0) {
				--backIdx;
				if (__builtin_expect(backIdx == -1), 0) {
					backIdx = cap - 1;
				}
			}
	#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
		}

		inline void updatePopBack() noexcept {
			--backIdx;

#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(unlikely)
			if (backIdx == -1) [[unlikely]] {
				backIdx = cap - 1;
			}
	#else
			if (__builtin_expect(backIdx == -1, 0)) {
				backIdx = cap - 1;
			}
	#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
		}

		inline void updatePopFront() noexcept {
			++frontIdx;

#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(unlikely)
			if (frontIdx == cap) [[unlikely]] {
				frontIdx = 0;
			}
	#else
			if (__builtin_expect(frontIdx == cap, 0)) {
				frontIdx = 0;
			}
	#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
		}

		inline std::int32_t getIdx (std::size_t idx) const noexcept {
			auto idx_ = static_cast<std::int32_t>(idx);
			idx_ += frontIdx;
			idx_ %= cap;
			return idx_;
		}
	};

	template <typename T>
	std::ostream& operator << (std::ostream& os, CircularBufferFixed<T> const& cb) {
		os << "[";
		bool first {true};
		for (std::size_t i = 0; i != cb.size(); ++i) {
			if (first) {
				first = false;
				os << cb.at(i);
				continue;
			}
			os << ", " << cb.at(i);
		}
		os << "]";
		return os;
	}

}//!namespace containers
