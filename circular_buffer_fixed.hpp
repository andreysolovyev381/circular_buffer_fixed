//
// Created by Elena Solovyeva on 02.12.2023.
//

#pragma once

#include <vector>
#include <concepts>
#include <type_traits>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace culib {

	namespace requirements {
		template <typename Numeric>
		concept is_index =
				std::convertible_to<Numeric, std::size_t> &&
				std::three_way_comparable<Numeric> &&
				requires (Numeric n) { n > 0; };
	}

	template <typename T>
	class circular_buffer_fixed_t {
	public:
		template <typename Numeric>
		requires requirements::is_index<Numeric>
		explicit circular_buffer_fixed_t (Numeric n)
				: last_idx {n - 1}
				, front_idx {0}
				, back_idx {0}
		{
			data.resize(n);
		}

		template <typename Numeric>
		requires requirements::is_index<Numeric> &&
				std::is_default_constructible_v<T>
		explicit circular_buffer_fixed_t (Numeric n, T default_value)
				: last_idx {n - 1}
				, front_idx {0}
				, back_idx {0}
		{
			data.resize(n, default_value);
		}

		void push(T t) {
			data[back_idx] = std::move(t);
			shift_back_idx();
		}
		void pop() {
			shift_front_idx();
		}

		T& front() {
			return data[front_idx];
		}

		T const& front() const {
			return data.at(front_idx);
		}

		T& back() {
			return data[back_idx];
		}

		T const& back() const {
			return data.at(back_idx);
		}

		template <typename Numeric>
		requires requirements::is_index<Numeric>
		T& at(Numeric idx) {
			if (idx < 0 || idx >= data.size()) {
				throw std::out_of_range("out of range with idx " + std::to_string(idx));
			}
			return data[idx];
		}
		template <typename Numeric>
		requires requirements::is_index<Numeric>
		T const& at(Numeric idx) const {
			if (idx < 0 || idx >= data.size()) {
				throw std::out_of_range("out of range with idx " + std::to_string(idx));
			}
			return data.at(idx);
		}

		template <typename Numeric>
		requires requirements::is_index<Numeric>
		T& operator[](Numeric idx) {
			return data[idx];
		}

	private:
		std::vector<T> data;
		std::int32_t const last_idx;
		std::int32_t front_idx, back_idx;

		void shift_back_idx() {
			if (last_idx == 0) return;
			if (back_idx == last_idx) back_idx = 0;
			else ++back_idx;

			if (back_idx == front_idx) ++back_idx;
		}
		void shift_front_idx() {
			if (last_idx == 0) return;
			if (front_idx == last_idx) front_idx = 0;
			else ++front_idx;

			if (front_idx == back_idx) ++front_idx;
		}
	};


}//!namespace
