#include <gtest/gtest.h>
#include "circular_buffer_fixed.hpp"

#include <string>
#include <sstream>

using namespace culib;

struct NonDefaultConstructible {
	explicit NonDefaultConstructible (int);
};

TEST(index, requirements) {

#ifdef __cpp_concepts
	bool maybe_index {culib::requirements::isIndex<std::string>};
	ASSERT_FALSE(maybe_index);

	maybe_index = culib::requirements::isIndex<int>;
	ASSERT_TRUE(maybe_index);

	maybe_index = culib::requirements::isIndex<NonDefaultConstructible>;
	ASSERT_FALSE(maybe_index);

#else
	//string as index
	bool convertible {std::is_convertible_v<std::string, culib::requirements::BaseIndexType>};
	ASSERT_FALSE(convertible);

	bool constructible = std::is_constructible_v<culib::requirements::BaseIndexType, std::string>;
	ASSERT_FALSE(constructible);

	bool three_way_comparable = std::conjunction_v<
			culib::requirements::SFINAE<std::string, std::equal_to<std::string>>,
			culib::requirements::SFINAE<std::string, std::less<std::string>>,
			culib::requirements::SFINAE<std::string, std::greater<std::string>>
	>;
	ASSERT_TRUE(three_way_comparable);

	bool compilable =
			std::conjunction_v<
					std::is_convertible<std::string, culib::requirements::BaseIndexType>,
					std::conjunction<
							culib::requirements::SFINAE<std::string, std::equal_to<std::string>>,
							culib::requirements::SFINAE<std::string, std::less<std::string>>,
							culib::requirements::SFINAE<std::string, std::greater<std::string>>
					>>;

	ASSERT_FALSE(compilable);

	bool is_not_void = {
			std::is_same_v<
					std::enable_if_t<
							!std::conjunction_v<
									std::is_convertible<std::string, culib::requirements::BaseIndexType>,
									std::conjunction<
											culib::requirements::SFINAE<std::string, std::equal_to<std::string>>,
											culib::requirements::SFINAE<std::string, std::less<std::string>>,
											culib::requirements::SFINAE<std::string, std::greater<std::string>>
									>>>,
					void>
	};
	ASSERT_TRUE(is_not_void);

	bool maybe_index = culib::requirements::isIndex<std::string>;
	ASSERT_FALSE(maybe_index);

	maybe_index = culib::requirements::isIndex<int>;
	ASSERT_TRUE(maybe_index);

	maybe_index = culib::requirements::isIndex<NonDefaultConstructible>;
	ASSERT_FALSE(maybe_index);

#endif

}

TEST(ctors, ctors_ok) {
	CircularBufferFixed<int> cb1 (5);
	ASSERT_EQ(cb1.front(), 0);
	ASSERT_EQ(cb1.back(), 0);
	CircularBufferFixed<int> cb2 (5, 42);
	ASSERT_EQ(cb2.front(), 42);
	ASSERT_EQ(cb2.back(), 42);
}

TEST(ctors, ctors_throw) {
	ASSERT_THROW(CircularBufferFixed<int>(0, 42), std::invalid_argument);
	ASSERT_THROW(CircularBufferFixed<int>(-2, 42), std::invalid_argument);
}

TEST(ctors, ctors_fail) {
	//no length provided
	bool is_contructible = std::is_constructible_v<CircularBufferFixed<int>>;
	ASSERT_FALSE(is_contructible);

#ifdef __cpp_concepts
	is_contructible = requires {requires std::constructible_from<CircularBufferFixed<NonDefaultConstructible>, int>;};
	ASSERT_FALSE(is_contructible);
#else
	//it just doesn't compile
#if 0
	//no default init
	is_contructible = std::is_constructible_v<CircularBufferFixed<NonDefaultConstructible>, int>;
	ASSERT_FALSE(is_contructible);
#endif
#endif

	ASSERT_FALSE(culib::requirements::isIndex<std::string>);
	is_contructible = std::is_constructible_v<CircularBufferFixed<int>, std::string>;
	ASSERT_FALSE(is_contructible);
}

TEST(update_data, pushBack) {
	CircularBufferFixed<int> cb (3, 42);
	cb.pushBack(1);
	ASSERT_EQ(cb.front(), 42);
	ASSERT_EQ(cb.back(), 1);
	cb.pushBack(2);
	ASSERT_EQ(cb.front(), 42);
	ASSERT_EQ(cb.back(), 2);
	cb.pushBack(3);
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 3);
	cb.pushBack(0);
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 0);
	cb.pushBack(17);
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 17);
	cb.pushBack(21);
	ASSERT_EQ(cb.front(), 0);
	ASSERT_EQ(cb.back(), 21);
	cb.pushBack(23);
	ASSERT_EQ(cb.front(), 17);
	ASSERT_EQ(cb.back(), 23);
}

TEST(update_data, pushFront) {
	CircularBufferFixed<int> cb (3, 42);
	cb.pushFront(1);
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 42);
	cb.pushFront(2);
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 42);
	cb.pushFront(3);
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 1);
	cb.pushFront(0);
	ASSERT_EQ(cb.front(), 0);
	ASSERT_EQ(cb.back(), 2);
	cb.pushFront(17);
	ASSERT_EQ(cb.front(), 17);
	ASSERT_EQ(cb.back(), 3);
	cb.pushFront(21);
	ASSERT_EQ(cb.front(), 21);
	ASSERT_EQ(cb.back(), 0);
	cb.pushFront(23);
	ASSERT_EQ(cb.front(), 23);
	ASSERT_EQ(cb.back(), 17);
}

TEST(update_data, popBack) {
	CircularBufferFixed<int> cb (3, 42);
	cb.pushBack(1);
	cb.pushBack(2);
	cb.pushBack(3);

	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 3);
	auto value = cb.popBack();
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 2);
	ASSERT_EQ(value, 3);
	value = cb.popBack();
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 1);
	ASSERT_EQ(value, 2);
	value = cb.popBack();
	ASSERT_EQ(value, 1);

	ASSERT_TRUE(cb.empty());
}

TEST(update_data, popFront) {
	CircularBufferFixed<int> cb (3, 42);
	cb.pushBack(1);
	cb.pushBack(2);
	cb.pushBack(3);

	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 3);
	auto value = cb.popFront();
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 3);
	ASSERT_EQ(value, 1);
	value = cb.popFront();
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 3);
	ASSERT_EQ(value, 2);
	value = cb.popFront();
	ASSERT_EQ(value, 3);
	ASSERT_TRUE(cb.empty());
}

TEST(update_data, mixed_modifications_1){
	CircularBufferFixed<int> cb(3, 42);
	cb.pushBack(1);
	cb.pushBack(2);
	cb.pushBack(3);
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 3);
	cb.popFront();
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 3);
	cb.pushBack(17);
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 17);
	cb.popFront();
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 17);
	cb.pushBack(21);
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 21);
	cb.pushBack(21);
	ASSERT_EQ(cb.front(), 17);
	ASSERT_EQ(cb.back(), 21);
	cb.pushFront(22);
	ASSERT_EQ(cb.front(), 22);
	ASSERT_EQ(cb.back(), 21);
	cb.pushFront(24);
	ASSERT_EQ(cb.front(), 24);
	ASSERT_EQ(cb.back(), 17);
	cb.popBack();
	ASSERT_EQ(cb.front(), 24);
	ASSERT_EQ(cb.back(), 22);
	cb.pushFront(17);
	ASSERT_EQ(cb.front(), 17);
	ASSERT_EQ(cb.back(), 22);
	cb.popBack();
	ASSERT_EQ(cb.front(), 17);
	ASSERT_EQ(cb.back(), 24);
	cb.popBack();
	ASSERT_EQ(cb.front(), 17);
	ASSERT_EQ(cb.back(), 17);
}

TEST(update_data, mixed_modifications_2) {
	CircularBufferFixed<int> cb(4);
	cb.pushFront(1);
	cb.pushFront(2);
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 1);
	int value = cb.popBack();
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 2);
	ASSERT_EQ(value, 1);
	value = cb.popBack();
	ASSERT_EQ(value, 2);
}

TEST(update_data, mixed_modifications_3) {
	CircularBufferFixed<int> cb(4);
	cb.pushFront(-5);
	cb.pushFront(0);
	int value = cb.popBack();
	ASSERT_EQ(value, -5);
	value = cb.popBack();
	ASSERT_EQ(value, 0);
	cb.pushBack(4);
	value = cb.popBack();
	ASSERT_EQ(value, 4);
	cb.pushBack(3);
}

TEST(update_data, mixed_modifications_4) {
	CircularBufferFixed<int> cb(8);
	cb.pushBack(-7);
	int value = cb.popBack();
	ASSERT_EQ(value, -7);
	ASSERT_EQ(cb.size(), 0u);
	value = cb.popBack();
	ASSERT_EQ(cb.size(), 0u);
	cb.pushFront(-6);
	ASSERT_EQ(cb.size(), 1u);
	value = cb.popBack();
	ASSERT_EQ(cb.size(), 0u);
	ASSERT_EQ(value, -6);
	cb.pushBack(1);
	ASSERT_EQ(cb.size(), 1u);
	value = cb.popFront();
	ASSERT_EQ(cb.size(), 0u);
	ASSERT_EQ(value, 1);
	value = cb.popBack();
	ASSERT_EQ(cb.size(), 0u);
	value = cb.popBack();
	ASSERT_EQ(cb.size(), 0u);
}
/*
TEST(update_data, mixed_modifications_5) {
	7
	7
	pop_front
			pop_front
	push_front 741
	push_front 648
	pop_front
			pop_back
	pop_front

			error
	error
	648
	741
	error

}
*/
TEST(access, method_at) {
	CircularBufferFixed<int> cb (3, 42);
	cb.pushBack(1);
	cb.pushBack(2);
	cb.pushBack(3);
	ASSERT_EQ(cb.at(0), 1);
	ASSERT_EQ(cb.at(1), 2);
	ASSERT_EQ(cb.at(2), 3);
}

TEST(access, operator_square_brackets) {
	CircularBufferFixed<int> cb (3, 42);
	cb.pushBack(1);
	cb.pushBack(2);
	cb.pushBack(3);
	ASSERT_EQ(cb[0], 1);
	ASSERT_EQ(cb[1], 2);
	ASSERT_EQ(cb[2], 3);
}

TEST(access, capacity) {
	CircularBufferFixed<int> cb1 (3, 42);
	ASSERT_EQ(cb1.capacity(), 3u);
	CircularBufferFixed<int> cb2 (2, 42);
	ASSERT_EQ(cb2.capacity(), 2u);
}

TEST(access, size) {
	CircularBufferFixed<int> cb (3);
	ASSERT_EQ(cb.size(), 0u);
	cb.pushBack(1);
	ASSERT_EQ(cb.size(), 1u);
	cb.pushBack(2);
	ASSERT_EQ(cb.size(), 2u);
	cb.pushBack(3);
	ASSERT_EQ(cb.size(), 3u);
	cb.pushBack(4);
	ASSERT_EQ(cb.size(), 3u);

	cb.popFront();
	ASSERT_EQ(cb.size(), 2u);
	cb.popFront();
	ASSERT_EQ(cb.size(), 1u);
	cb.popFront();
	ASSERT_EQ(cb.size(), 0u);
	cb.popFront();
	ASSERT_EQ(cb.size(), 0u);

	cb.pushFront(1);
	ASSERT_EQ(cb.size(), 1u);
	cb.pushFront(2);
	ASSERT_EQ(cb.size(), 2u);
	cb.pushFront(3);
	ASSERT_EQ(cb.size(), 3u);
	cb.pushFront(4);
	ASSERT_EQ(cb.size(), 3u);

	cb.popBack();
	ASSERT_EQ(cb.size(), 2u);
	cb.popBack();
	ASSERT_EQ(cb.size(), 1u);
	cb.popBack();
	ASSERT_EQ(cb.size(), 0u);
	cb.popBack();
	ASSERT_EQ(cb.size(), 0u);
}

TEST(access, empty) {
	int const capacity {3};
	CircularBufferFixed<int> cb (capacity);
	ASSERT_TRUE(cb.empty());
	cb.pushBack(1);
	ASSERT_FALSE(cb.empty());
	cb.pushBack(2);
	ASSERT_FALSE(cb.empty());
	cb.pushBack(3);
	ASSERT_FALSE(cb.empty());
	cb.pushBack(4);
	ASSERT_FALSE(cb.empty());

	for (int i = 0; i != capacity - 1; ++i){
		cb.popBack();
		ASSERT_FALSE(cb.empty());
	}
	cb.popBack();
	ASSERT_TRUE(cb.empty());
	cb.popBack();
	ASSERT_TRUE(cb.empty());

	cb.pushFront(1);
	ASSERT_FALSE(cb.empty());
	cb.pushFront(2);
	ASSERT_FALSE(cb.empty());
	cb.pushFront(3);
	ASSERT_FALSE(cb.empty());
	cb.pushFront(4);
	ASSERT_FALSE(cb.empty());

	for (int i = 0; i != capacity - 1; ++i){
		cb.popFront();
		ASSERT_FALSE(cb.empty());
	}
	cb.popFront();
	ASSERT_TRUE(cb.empty());
	cb.popFront();
	ASSERT_TRUE(cb.empty());
}

TEST(special_case, size_of_one) {
	CircularBufferFixed<int> cb (1, 42);
	cb.pushBack(1);
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 1);
	ASSERT_EQ(cb.at(0), 1);
	ASSERT_EQ(cb[0], 1);
	cb.pushBack(2);
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 2);
	ASSERT_EQ(cb.at(0), 2);
	ASSERT_EQ(cb[0], 2);
	cb.pushFront(3);
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 3);
	ASSERT_EQ(cb.at(0), 3);
	ASSERT_EQ(cb[0], 3);
}

TEST(stream, ostream) {
	CircularBufferFixed<int> cb (3, 0);
	cb.pushBack(1);
	cb.pushBack(2);
	cb.pushBack(3);

	std::stringstream ss;
	ss << cb;
	std::string_view check {"[1, 2, 3]"};
#if __cplusplus < 201803L
	ASSERT_EQ(ss.str(), check);
#else
	ASSERT_EQ(ss.view(), check);
#endif
}

TEST(iterating, using_at ) {
	CircularBufferFixed<int> cb (3, 42);
	std::vector<int> v {1, 2, 3};
	for (auto const elem : v) {
		cb.pushBack(elem);
	}

	//logic state is {1, 2, 3}, real state is {1, 2, 3}
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb.at(i), v.at(i));
	}

	cb.pushBack(0); //logic state is {2, 3, 0}, real state is {0, 2, 3}
	v = {2, 3, 0};
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb.at(i), v.at(i));
	}

	cb.popFront();
	cb.pushBack(1); //logic state is {3, 0, 1}, real state is {0, 1, 3}
	v = {3, 0, 1};
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb.at(i), v.at(i));
	}
}

TEST(iterating, using_sq_brackets ) {
	CircularBufferFixed<int> cb (3, 42);
	std::vector<int> v {1, 2, 3};
	for (auto const elem : v) {
		cb.pushBack(elem);
	}

	//logic state is {1, 2, 3}, real state is {1, 2, 3}
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb[i], v[i]);
	}

	cb.pushBack(0); //logic state is {2, 3, 0}, real state is {0, 2, 3}
	v = {2, 3, 0};
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb[i], v[i]);
	}

	cb.popFront();
	cb.pushBack(1); //logic state is {3, 0, 1}, real state is {0, 1, 3}
	v = {3, 0, 1};
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb[i], v[i]);
	}
}

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	testing::GTEST_FLAG(color) = "yes";

	return RUN_ALL_TESTS();
}
