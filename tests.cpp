#include <gtest/gtest.h>
#include "circular_buffer_fixed.hpp"

#include <string>
#include <sstream>

using namespace culib;

//#define COMPILE_TIME_FAILURE

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

#if defined(COMPILE_TIME_FAILURE)
TEST(ctors, ctors_fail) {
	CircularBufferFixed<int> cb1;

	struct NonDefaultConstructible {
		explicit NonDefaultConstructible (int);
	};
	CircularBufferFixed<NonDefaultConstructible> cb2(2);

	std::string index {"1"};
	CircularBufferFixed<int> cb3(index);
}
#endif

TEST(update_data, push) {
	CircularBufferFixed<int> cb (3, 42);
	cb.push(1);
	ASSERT_EQ(cb.front(), 42);
	ASSERT_EQ(cb.back(), 1);
	cb.push(2);
	ASSERT_EQ(cb.front(), 42);
	ASSERT_EQ(cb.back(), 2);
	cb.push(3);
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 3);
	cb.push(0);
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 0);
}

TEST(update_data, pop) {
	CircularBufferFixed<int> cb (3, 42);
	cb.push(1);
	cb.push(2);
	cb.push(3);

	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 3);
	auto value = cb.pop();
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 3);
	ASSERT_EQ(value, 1);
	value = cb.pop();
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 3);
	ASSERT_EQ(value, 2);
	value = cb.pop();
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 3);
	ASSERT_EQ(value, 3);
}

TEST(update_data, push_and_pop) {
	CircularBufferFixed<int> cb (3, 42);
	cb.push(1);
	cb.push(2);
	cb.push(3);
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 3);
	cb.pop();
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 3);
	cb.push(17);
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 17);
	cb.pop();
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 17);
	cb.push(21);
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 21);
}

TEST(access, method_at) {
	CircularBufferFixed<int> cb (3, 42);
	cb.push(1);
	cb.push(2);
	cb.push(3);
	ASSERT_EQ(cb.at(0), 1);
	ASSERT_EQ(cb.at(1), 2);
	ASSERT_EQ(cb.at(2), 3);
}

TEST(access, operator_square_brackets) {
	CircularBufferFixed<int> cb (3, 42);
	cb.push(1);
	cb.push(2);
	cb.push(3);
	ASSERT_EQ(cb[0], 1);
	ASSERT_EQ(cb[1], 2);
	ASSERT_EQ(cb[2], 3);
}

TEST(access, size) {
	CircularBufferFixed<int> cb1 (3, 42);
	ASSERT_EQ(cb1.size(), 3u);
	CircularBufferFixed<int> cb2 (2, 42);
	ASSERT_EQ(cb2.size(), 2u);
}

TEST(special_case, size_of_one) {
	CircularBufferFixed<int> cb (1, 42);
	cb.push(1);
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 1);
	ASSERT_EQ(cb.at(0), 1);
	ASSERT_EQ(cb[0], 1);
	cb.push(2);
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 2);
	ASSERT_EQ(cb.at(0), 2);
	ASSERT_EQ(cb[0], 2);
}

TEST(stream, ostream) {
	CircularBufferFixed<int> cb (3, 0);
	cb.push(1);
	cb.push(2);
	cb.push(3);

	std::stringstream ss;
	ss << cb;
	std::string_view check {"{1, 2, 3}"};
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
		cb.push(elem);
	}

	//logic state is {1, 2, 3}, real state is {1, 2, 3}
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb.at(i), v.at(i));
	}

	cb.push(0); //logic state is {2, 3, 0}, real state is {0, 2, 3}
	v = {2, 3, 0};
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb.at(i), v.at(i));
	}

	cb.pop();
	cb.push(1); //logic state is {3, 0, 1}, real state is {0, 1, 3}
	v = {3, 0, 1};
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb.at(i), v.at(i));
	}
}

TEST(iterating, using_sq_brackets ) {
	CircularBufferFixed<int> cb (3, 42);
	std::vector<int> v {1, 2, 3};
	for (auto const elem : v) {
		cb.push(elem);
	}

	//logic state is {1, 2, 3}, real state is {1, 2, 3}
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb[i], v[i]);
	}

	cb.push(0); //logic state is {2, 3, 0}, real state is {0, 2, 3}
	v = {2, 3, 0};
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb[i], v[i]);
	}

	cb.pop();
	cb.push(1); //logic state is {3, 0, 1}, real state is {0, 1, 3}
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
