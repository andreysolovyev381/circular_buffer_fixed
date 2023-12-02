//
// Created by Elena Solovyeva on 02.12.2023.
//

#pragma once

#include <vector>
#include <array>
#include <concepts>
#include <type_traits>
#include <cstddef>
#include <stdexcept>
#include <utility>

#include <immintrin.h>

namespace culib {

	namespace const_values {
		static std::size_t constexpr maxMemSizeForStack {1 << 17};
	}//!namespace

	namespace requirements {
		template <typename Numeric>
		concept IsIndex =
				std::convertible_to<Numeric, std::size_t> &&
				std::three_way_comparable<Numeric>;

		template <typename T, template <typename> typename Container, int N = 1>
		concept IsSequential =
				std::same_as<Container<T>, std::vector<T>> ||
				std::same_as<Container<T>, std::array<T, N>>;
	}//!namespace

	template <typename T>
	class CircularBufferFixed {
	public:
		using value_type = T;
		template <typename Numeric>
		requires requirements::IsIndex<Numeric>
		explicit CircularBufferFixed (Numeric n, T defaultValue)
				: sz 		{static_cast<std::int32_t>(n)}
				, frontIdx 	{0}
				, backIdx 	{static_cast<std::int32_t>(n) - 1}
		{
			if (n < 1) throw std::invalid_argument ("Can't create a fixed size circular buffer with size 0. Just why?..");
			data.resize(n, defaultValue);
		}

		void push(T t) noexcept {
			updatePush();
			_mm_prefetch((const void *)(&data[backIdx]), _MM_HINT_T0);
			_mm_prefetch((const void *)(&t), _MM_HINT_T0);
			data[backIdx] = std::move(t);
		}

		T pop() noexcept {
			T res = data[frontIdx];
			updatePop();
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

		template <typename Numeric>
		requires requirements::IsIndex<Numeric>
		T& at(Numeric idx) {
			auto idx_ = getIdx(idx);
			if (idx_ >= sz) {
				throw std::out_of_range("out of range with idx " + std::to_string(idx));
			}
			return data[idx_];
		}

		template <typename Numeric>
		requires requirements::IsIndex<Numeric>
		T const& at(Numeric idx) const {
			auto idx_ = getIdx(idx);
			if (idx_ >= sz) {
				throw std::out_of_range("out of range with idx " + std::to_string(idx));
			}
			return data.at(idx_);
		}

		template <typename Numeric>
		requires requirements::IsIndex<Numeric>
		T& operator[](Numeric idx) noexcept {
			auto idx_ = getIdx(idx);
			return data[idx_];
		}

		std::size_t size() const noexcept {
			return sz;
		}

	private:
		std::vector<T> data;
		std::int32_t const sz;
		std::int32_t frontIdx, backIdx;

		inline void updatePush() noexcept {
			++backIdx;
			if (backIdx == sz) [[unlikely]] {
				backIdx = 0;
			}
			if (backIdx == frontIdx) [[unlikely]] {
				++frontIdx;
				if (frontIdx == sz) [[unlikely]] {
					frontIdx = 0;
				}
			}
		}

		inline void updatePop() noexcept {
			++frontIdx;
			if (frontIdx == sz) [[unlikely]] {
				frontIdx = 0;
			}
		}

		template <typename Numeric>
		inline std::int32_t getIdx (Numeric idx) const noexcept {
			auto idx_ = static_cast<std::int32_t>(idx);
			idx_ += frontIdx;
			idx_ %= sz;
			return idx_;
		}
	};

	template <typename T>
	std::ostream& operator << (std::ostream& os, CircularBufferFixed<T> const& cb) {
		os << "{";
		bool first {true};
		for (std::size_t i = 0; i != cb.size(); ++i) {
			if (first) {
				first = false;
				os << cb.at(i);
				continue;
			}
			os << ", " << cb.at(i);
		}
		os << "}";
		return os;
	}

}//!namespace
