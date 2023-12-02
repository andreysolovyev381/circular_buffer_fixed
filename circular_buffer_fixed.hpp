//
// Created by Elena Solovyeva on 02.12.2023.
//

#pragma once

#include <vector>
#include <concepts>
#include <type_traits>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace culib {

	namespace const_values {
		static std::size_t constexpr maxSizeForStack {1 << 17};
	}//!namespace

	namespace requirements {
		template <typename Numeric>
		concept IsIndex =
				std::convertible_to<Numeric, std::size_t> &&
				std::three_way_comparable<Numeric>;
	}//!namespace

	template <typename T>
	class CircularBufferFixed {
	public:
		template <typename Numeric>
		requires requirements::IsIndex<Numeric>
		explicit CircularBufferFixed (Numeric n, T defaultValue)
				: sz 		{static_cast<std::int32_t>(n)}
				, frontIdx {0}
				, backIdx 	{static_cast<std::int32_t>(n) - 1}
		{
			if (n < 1) throw std::invalid_argument ("Can't create a fixed size circular buffer with size 0. Just why?..");
			data.resize(n, defaultValue);
		}

		void push(T t) {
			updatePush();
			data[backIdx] = std::move(t);
		}

		T pop() {
			T res = data[frontIdx];
			updatePop();
			return res;
		}

		T& front() {
			return data[frontIdx];
		}

		T const& front() const {
			return data.at(frontIdx);
		}

		T& back() {
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
		T& operator[](Numeric idx) {
			auto idx_ = getIdx(idx);
			return data[idx_];
		}

		std::size_t size() const {
			return sz;
		}

	private:
		std::vector<T> data;
		std::int32_t const sz;
		std::int32_t frontIdx, backIdx;

		inline void updatePush () {
			if (sz == 1) {
				return;
			}
			if (backIdx == sz - 1) {
				backIdx = 0;
			}
			else {
				++backIdx;
			}
			if (backIdx == frontIdx) {
				++frontIdx;
				if (frontIdx == sz) {
					frontIdx = 0;
				}
			}
		}
		inline void updatePop () {
			if (sz == 1) {
				return;
			}
			if (frontIdx == sz - 1) {
				frontIdx = 0;
			}
			else {
				++frontIdx;
			}
		}

		template <typename Numeric>
		inline std::int32_t getIdx (Numeric idx) const {
			auto idx_ = static_cast<std::int32_t>(idx);
			idx_ += frontIdx;
			idx %= sz;
			return idx;
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
