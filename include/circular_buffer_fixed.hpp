//
// Created by Andrey Solovyev on 02.12.2023.
//

#pragma once

#include <iostream>
#include <array>
#include <type_traits>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <concepts>

namespace containers {

	template <typename T, std::int32_t N>
	requires (requires {N > 0;})
	class CircularBufferFixed {
	public:
		using container_type = CircularBufferFixed<T, 0>;
		using value_type = T;

		explicit CircularBufferFixed ()
		requires std::is_default_constructible_v<T>
				: sz        {0}
				, front_idx {0}
				, back_idx 	{0}
		{
			data.fill(T{});
		}

		explicit CircularBufferFixed (T defaultValue)
				: sz        {N}
				, front_idx {0}
				, back_idx 	{N - 1}
		{
			data.fill(defaultValue);
		}

		void push_back(T t) noexcept {
#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(likely)
			if (sz != 0u) [[likely]] {
				update_on_push_back();
			}
			if (sz < N) [[likely]] {
				++sz;
			}
	#else
			if (__builtin_expect(sz != 0u, 1)) {
				update_on_push_back();
			}
			if (__builtin_expect(sz < cap), 1){
				++sz;
			}
#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
			data[back_idx] = std::move(t);
		}

		void push_front(T t) noexcept {
#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(likely)
			if (sz != 0u) [[likely]] {
				update_on_push_front();
			}
			if (sz < N) [[likely]] {
				++sz;
			}
	#else
			if (__builtin_expect(sz != 0u, 1)) {
				update_on_push_front();
			}
			if (__builtin_expect(sz < cap), 1) {
				++sz;
			}
#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
			data[front_idx] = std::move(t);
		}

		T& pop_back() noexcept {
			T& res = data[back_idx];
#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(likely)
			if (sz != 1 && front_idx != back_idx) [[likely]] {
				update_on_pop_back();
			}
			if (sz > 0) [[likely]] {
				--sz;
			}
	#else
			if (__builtin_expect(sz != 1 && front_idx != back_idx), 1) {
				update_on_pop_back();
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

		T& pop_front() noexcept {
			T& res = data[front_idx];
#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(likely)
			if (sz != 1 && front_idx != back_idx) [[likely]] {
				update_on_pop_front();
			}
			if (sz > 0)  [[likely]] {
				--sz;
			}
	#else
			if (__builtin_expect(sz != 1 && front_idx != back_idx), 1) {
				update_on_pop_front();
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
			return data[front_idx];
		}

		T const& front() const {
			return data.at(front_idx);
		}

		T& back() noexcept {
			return data[back_idx];
		}

		T const& back() const {
			return data.at(back_idx);
		}

		T& at(std::size_t idx) {
			auto idx_ = get_idx(idx);
			if (idx_ >= N) {
				throw std::out_of_range("CircularBufferFixed is out of range with idx " + std::to_string(idx));
			}
			return data[idx_];
		}

		T const& at(std::size_t idx) const {
			auto idx_ = get_idx(idx);
			if (idx_ >= N) {
				throw std::out_of_range("CircularBufferFixed is out of range with idx " + std::to_string(idx));
			}
			return data.at(idx_);
		}

		T& operator[](std::size_t idx) noexcept {
			auto idx_ = get_idx(idx);
			return data[idx_];
		}

		static constexpr std::int32_t capacity() noexcept {
			return N;
		}

		std::size_t size() const noexcept {
			return sz;
		}

		bool empty() const noexcept {
			return sz == 0u;
		}

	private:
		std::array<T, N> data;
		std::int32_t sz;
		std::int32_t front_idx, back_idx;

		inline void update_on_push_back() noexcept {
			++back_idx;

#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(unlikely)
			if (back_idx == N) [[unlikely]] {
				back_idx = 0;
			}
			if (back_idx == front_idx) [[unlikely]] {
				++front_idx;
				if (front_idx == N) [[unlikely]] {
					front_idx = 0;
				}
			}
	#else
			if (__builtin_expect(back_idx == cap, 0)) {
				back_idx = 0;
			}
			if (__builtin_expect(back_idx == front_idx, 0)) {
				++front_idx;
				if (__builtin_expect(front_idx == cap, 0)) {
					front_idx = 0;
				}
			}
	#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
		}

		void update_on_push_front() noexcept {
			--front_idx;
#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(unlikely)
			if (front_idx == -1) [[unlikely]] {
				front_idx = N - 1;
			}
			if(front_idx == back_idx) [[unlikely]] {
				--back_idx;
				if (back_idx == -1) [[unlikely]] {
					back_idx = N - 1;
				}
			}
	#else
			if (__builtin_expect(front_idx == -1), 0){
				front_idx = cap - 1;
			}
			if(__builtin_expect(front_idx == back_idx), 0) {
				--back_idx;
				if (__builtin_expect(back_idx == -1), 0) {
					back_idx = cap - 1;
				}
			}
	#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
		}

		inline void update_on_pop_back() noexcept {
			--back_idx;

#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(unlikely)
			if (back_idx == -1) [[unlikely]] {
				back_idx = N - 1;
			}
	#else
			if (__builtin_expect(back_idx == -1, 0)) {
				back_idx = cap - 1;
			}
	#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
		}

		inline void update_on_pop_front() noexcept {
			++front_idx;

#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(unlikely)
			if (front_idx == N) [[unlikely]] {
				front_idx = 0;
			}
	#else
			if (__builtin_expect(front_idx == cap, 0)) {
				front_idx = 0;
			}
	#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
		}

		inline std::int32_t get_idx (std::size_t idx) const noexcept {
			auto idx_ = static_cast<std::int32_t>(idx);
			idx_ += front_idx;
			idx_ %= N;
			return idx_;
		}
	};

}//!namespace containers
