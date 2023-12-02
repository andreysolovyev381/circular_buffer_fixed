//
// Created by Andrey Solovyev on 27/03/2023.
//

#include <gtest/gtest.h>
#include "circular_buffer_fixed.hpp"

#include <string>
#include <sstream>

using namespace culib;

//#define COMPILE_TIME_FAILURE

TEST(cb_fixed, successfull_creation) {
	CircularBufferFixed<int> cb1 (5);
	ASSERT_EQ(cb1.front(), 0);
	ASSERT_EQ(cb1.back(), 0);
	CircularBufferFixed<int> cb2 (5, 42);
	ASSERT_EQ(cb2.front(), 42);
	ASSERT_EQ(cb2.back(), 42);

	ASSERT_THROW(CircularBufferFixed<int>(-2, 42), std::invalid_argument);
}

#if defined(COMPILE_TIME_FAILURE)
TEST(cb_fixed, failed_creation) {
	CircularBufferFixed<int> cb1;
	std::string index {"1"};
	CircularBufferFixed<int> cb3(index);
}
#endif

TEST(cb_fixed, push) {
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

TEST(cb_fixed, pop) {
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

TEST(cb_fixed, push_and_pop) {
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

TEST(cb_fixed, method_at) {
	CircularBufferFixed<int> cb (3, 42);
	cb.push(1);
	cb.push(2);
	cb.push(3);
	ASSERT_EQ(cb.at(0), 1);
	ASSERT_EQ(cb.at(1), 2);
	ASSERT_EQ(cb.at(2), 3);
}

TEST(cb_fixed, operator_square_brackets) {
	CircularBufferFixed<int> cb (3, 42);
	cb.push(1);
	cb.push(2);
	cb.push(3);
	ASSERT_EQ(cb[0], 1);
	ASSERT_EQ(cb[1], 2);
	ASSERT_EQ(cb[2], 3);
}

TEST(cb_fixed, size) {
	CircularBufferFixed<int> cb1 (3, 42);
	ASSERT_EQ(cb1.size(), 3u);
	CircularBufferFixed<int> cb2 (2, 42);
	ASSERT_EQ(cb2.size(), 2u);
}

TEST(cb_fixed, size_of_one) {
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

TEST(cb_fixed, ostream_operator) {
	CircularBufferFixed<int> cb (3, 0);
	cb.push(1);
	cb.push(2);
	cb.push(3);

	std::stringstream ss;
	ss << cb;
	std::string_view check {"{1, 2, 3}"};

	ASSERT_EQ(ss.view(), check);
}
