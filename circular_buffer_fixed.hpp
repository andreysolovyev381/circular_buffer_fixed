//
// Created by Andrey Solovyev on 02.12.2023.
//

#pragma once

#include <vector>
#include <array>
#include <type_traits>
#include <cstddef>
#include <stdexcept>
#include <utility>

#ifdef __cpp_concepts
#include <concepts>
#endif


namespace culib {

	namespace requirements {
		using BaseIndexType = std::size_t;
#ifdef __cpp_concepts
		template <typename Numeric>
		concept IsIndex =
		std::convertible_to<Numeric, BaseIndexType> &&
		std::three_way_comparable<Numeric>;
#else

		template <typename Numeric, typename = void>
		struct MaybeIndex : std::false_type {};

		template <typename Numeric>
		struct MaybeIndex<Numeric, std::void_t<
				std::conjunction<
					std::is_convertible<Numeric, BaseIndexType>,
					std::conjunction<
						decltype(std::declval<std::equal_to<Numeric>>()),
						decltype(std::declval<std::less<Numeric>>()),
						decltype(std::declval<std::greater<Numeric>>())
									>
								>
				>> : std::true_type {};

		template <typename Numeric>
		static constexpr bool isIndex {MaybeIndex<Numeric>::value};

		template <typename Numeric>
		using IsIndex = std::enable_if_t<isIndex<Numeric>, bool>;

		template <typename DataType>
		using IsDefaultConstructible = std::enable_if_t<std::is_default_constructible_v<DataType>, bool>;
#endif
	}//!namespace

	template <typename T>
	class CircularBufferFixed {
	public:
		using value_type = T;

#ifdef __cpp_concepts
		template <typename Numeric>
		requires requirements::IsIndex<Numeric> && std::is_default_constructible_v<T>
#else
		template <typename Numeric,
				requirements::IsIndex<Numeric> = true,
				requirements::IsDefaultConstructible<T> = true>
#endif
		explicit CircularBufferFixed (Numeric n)
				: sz 		{static_cast<std::int32_t>(n)}
				, frontIdx 	{0}
				, backIdx 	{static_cast<std::int32_t>(n) - 1}
		{
			if (n < 1) throw std::invalid_argument ("Can't create a fixed size circular buffer with size 0. Just why?..");
			data.resize(n, T{});
		}

#ifdef __cpp_concepts
		template <typename Numeric>
		requires requirements::IsIndex<Numeric>
#else
		template <typename Numeric,
				requirements::IsIndex<Numeric> = true>
#endif
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
			data[backIdx] = std::move(t);
		}

		T& pop() noexcept {
			T& res = data[frontIdx];
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

#ifdef __cpp_concepts
		template <typename Numeric>
		requires requirements::IsIndex<Numeric>
#else
		template <typename Numeric,
				requirements::IsIndex<Numeric> = true>
#endif
		T& at(Numeric idx) {
			auto idx_ = getIdx(idx);
			if (idx_ >= sz) {
				throw std::out_of_range("CircularBufferFixed is out of range with idx " + std::to_string(idx));
			}
			return data[idx_];
		}

#ifdef __cpp_concepts
		template <typename Numeric>
		requires requirements::IsIndex<Numeric>
#else
		template <typename Numeric,
				requirements::IsIndex<Numeric> = true>
#endif
		T const& at(Numeric idx) const {
			auto idx_ = getIdx(idx);
			if (idx_ >= sz) {
				throw std::out_of_range("CircularBufferFixed is out of range with idx " + std::to_string(idx));
			}
			return data.at(idx_);
		}

#ifdef __cpp_concepts
		template <typename Numeric>
		requires requirements::IsIndex<Numeric>
#else
		template <typename Numeric,
				requirements::IsIndex<Numeric> = true>
#endif
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

#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(unlikely)
			if (backIdx == sz) [[unlikely]] {
				backIdx = 0;
			}
			if (backIdx == frontIdx) [[unlikely]] {
				++frontIdx;
				if (frontIdx == sz) [[unlikely]] {
					frontIdx = 0;
				}
			}
	#else
			if (__builtin_expect(backIdx == sz, 0)) {
				backIdx = 0;
			}
			if (__builtin_expect(backIdx == frontIdx, 0)) {
				++frontIdx;
				if (__builtin_expect(frontIdx == sz, 0)) {
					frontIdx = 0;
				}
			}
	#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
		}

		inline void updatePop() noexcept {
			++frontIdx;

#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(unlikely)
			if (frontIdx == sz) [[unlikely]] {
				frontIdx = 0;
			}
	#else
			if (__builtin_expect(frontIdx == sz, 0)) {
				frontIdx = 0;
			}
	#endif
#else
			throw std::runtime_error ("This is some kind of a wrong C++. Check your planet.");
#endif
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
