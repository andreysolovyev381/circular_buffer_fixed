//
// Created by Andrey Solovyev on 02.12.2023.
//

#pragma once

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


namespace culib {

	namespace requirements {
		using BaseIndexType = std::size_t;
#ifdef __cpp_concepts
		template <typename Numeric>
		concept IsIndex =
		std::convertible_to<Numeric, BaseIndexType> &&
		std::three_way_comparable<Numeric>;

		template <typename Numeric>
		static inline constexpr bool isIndex {IsIndex<Numeric> ? true : false};

#else

		template <typename T, typename Cond, typename = void>
		struct SFINAE : std::false_type {};

		template <typename T, typename Cond>
		struct SFINAE<T, Cond, std::void_t<Cond>> : std::true_type {};

		template <typename Numeric, typename = void>
		struct MaybeIndex : std::false_type {};

		template <typename Numeric>
		struct MaybeIndex<Numeric,
				std::enable_if_t<std::conjunction_v<
					std::is_convertible<BaseIndexType, Numeric>,
					std::conjunction<
						SFINAE<Numeric, std::equal_to<Numeric>>,
						SFINAE<Numeric, std::less<Numeric>>,
						SFINAE<Numeric, std::greater<Numeric>>
									>>>>
									: std::true_type {};

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
		template <requirements::IsIndex Numeric>
		requires std::is_default_constructible_v<T>
#else
		template <typename Numeric,
				requirements::IsIndex<Numeric> = true,
				requirements::IsDefaultConstructible<T> = true>
#endif
		explicit CircularBufferFixed (Numeric n)
				: cap 		{static_cast<std::int32_t>(n)}
				, sz        {0}
				, frontIdx 	{0}
				, backIdx 	{0}
		{
			if (n < 1) throw std::invalid_argument ("Can't create a fixed size circular buffer with size 0. Just why?..");
			data.resize(n, T{});
		}

#ifdef __cpp_concepts
		template <requirements::IsIndex Numeric>
#else
		template <typename Numeric, requirements::IsIndex<Numeric> = true>
#endif
		explicit CircularBufferFixed (Numeric n, T defaultValue)
				: cap 		{static_cast<std::int32_t>(n)}
				, sz        {static_cast<std::int32_t>(n)}
				, frontIdx 	{0}
				, backIdx 	{static_cast<std::int32_t>(n) - 1}
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

#ifdef __cpp_concepts
		template <requirements::IsIndex Numeric>
#else
		template <typename Numeric, requirements::IsIndex<Numeric> = true>
#endif
		T& at(Numeric idx) {
			auto idx_ = getIdx(idx);
			if (idx_ >= cap) {
				throw std::out_of_range("CircularBufferFixed is out of range with idx " + std::to_string(idx));
			}
			return data[idx_];
		}

#ifdef __cpp_concepts
		template <requirements::IsIndex Numeric>
#else
		template <typename Numeric, requirements::IsIndex<Numeric> = true>
#endif
		T const& at(Numeric idx) const {
			auto idx_ = getIdx(idx);
			if (idx_ >= cap) {
				throw std::out_of_range("CircularBufferFixed is out of range with idx " + std::to_string(idx));
			}
			return data.at(idx_);
		}

#ifdef __cpp_concepts
		template <requirements::IsIndex Numeric>
#else
		template <typename Numeric, requirements::IsIndex<Numeric> = true>
#endif
		T& operator[](Numeric idx) noexcept {
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

		template <typename Numeric>
		inline std::int32_t getIdx (Numeric idx) const noexcept {
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

}//!namespace
