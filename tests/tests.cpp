#include <gtest/gtest.h>
#include "../include/circular_buffer_fixed.hpp"

#include <string>
#include <sstream>

using namespace containers;

struct NonDefaultConstructible {
	explicit NonDefaultConstructible (int);
};

TEST(ctors, ctors_ok) {
	CircularBufferFixed<int, 5> cb1;
	ASSERT_EQ(cb1.front(), 0);
	ASSERT_EQ(cb1.back(), 0);
	CircularBufferFixed<int, 5> cb2 (42);
	ASSERT_EQ(cb2.front(), 42);
	ASSERT_EQ(cb2.back(), 42);
}

TEST(update_data, push_back) {
	CircularBufferFixed<int, 3> cb (42);
	cb.push_back(1);
	ASSERT_EQ(cb.front(), 42);
	ASSERT_EQ(cb.back(), 1);
	cb.push_back(2);
	ASSERT_EQ(cb.front(), 42);
	ASSERT_EQ(cb.back(), 2);
	cb.push_back(3);
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 3);
	cb.push_back(0);
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 0);
	cb.push_back(17);
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 17);
	cb.push_back(21);
	ASSERT_EQ(cb.front(), 0);
	ASSERT_EQ(cb.back(), 21);
	cb.push_back(23);
	ASSERT_EQ(cb.front(), 17);
	ASSERT_EQ(cb.back(), 23);
}

TEST(update_data, push_front) {
	CircularBufferFixed<int, 3> cb (42);
	cb.push_front(1);
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 42);
	cb.push_front(2);
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 42);
	cb.push_front(3);
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 1);
	cb.push_front(0);
	ASSERT_EQ(cb.front(), 0);
	ASSERT_EQ(cb.back(), 2);
	cb.push_front(17);
	ASSERT_EQ(cb.front(), 17);
	ASSERT_EQ(cb.back(), 3);
	cb.push_front(21);
	ASSERT_EQ(cb.front(), 21);
	ASSERT_EQ(cb.back(), 0);
	cb.push_front(23);
	ASSERT_EQ(cb.front(), 23);
	ASSERT_EQ(cb.back(), 17);
}

TEST(update_data, popBack) {
	CircularBufferFixed<int, 3> cb (42);
	cb.push_back(1);
	cb.push_back(2);
	cb.push_back(3);

	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 3);
	auto value = cb.pop_back();
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 2);
	ASSERT_EQ(value, 3);
	value = cb.pop_back();
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 1);
	ASSERT_EQ(value, 2);
	value = cb.pop_back();
	ASSERT_EQ(value, 1);

	ASSERT_TRUE(cb.empty());
}

TEST(update_data, popFront) {
	CircularBufferFixed<int, 3> cb (42);
	cb.push_back(1);
	cb.push_back(2);
	cb.push_back(3);

	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 3);
	auto value = cb.pop_front();
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 3);
	ASSERT_EQ(value, 1);
	value = cb.pop_front();
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 3);
	ASSERT_EQ(value, 2);
	value = cb.pop_front();
	ASSERT_EQ(value, 3);
	ASSERT_TRUE(cb.empty());
}

TEST(update_data, mixed_modifications_1){
	CircularBufferFixed<int, 3> cb(42);
	cb.push_back(1);
	cb.push_back(2);
	cb.push_back(3);
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 3);
	cb.pop_front();
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 3);
	cb.push_back(17);
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 17);
	cb.pop_front();
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 17);
	cb.push_back(21);
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 21);
	cb.push_back(21);
	ASSERT_EQ(cb.front(), 17);
	ASSERT_EQ(cb.back(), 21);
	cb.push_front(22);
	ASSERT_EQ(cb.front(), 22);
	ASSERT_EQ(cb.back(), 21);
	cb.push_front(24);
	ASSERT_EQ(cb.front(), 24);
	ASSERT_EQ(cb.back(), 17);
	cb.pop_back();
	ASSERT_EQ(cb.front(), 24);
	ASSERT_EQ(cb.back(), 22);
	cb.push_front(17);
	ASSERT_EQ(cb.front(), 17);
	ASSERT_EQ(cb.back(), 22);
	cb.pop_back();
	ASSERT_EQ(cb.front(), 17);
	ASSERT_EQ(cb.back(), 24);
	cb.pop_back();
	ASSERT_EQ(cb.front(), 17);
	ASSERT_EQ(cb.back(), 17);
}

TEST(update_data, mixed_modifications_2) {
	CircularBufferFixed<int, 4> cb;
	cb.push_front(1);
	cb.push_front(2);
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 1);
	int value = cb.pop_back();
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 2);
	ASSERT_EQ(value, 1);
	value = cb.pop_back();
	ASSERT_EQ(value, 2);
}

TEST(update_data, mixed_modifications_3) {
	CircularBufferFixed<int,4 > cb;
	cb.push_front(-5);
	cb.push_front(0);
	int value = cb.pop_back();
	ASSERT_EQ(value, -5);
	value = cb.pop_back();
	ASSERT_EQ(value, 0);
	cb.push_back(4);
	value = cb.pop_back();
	ASSERT_EQ(value, 4);
	cb.push_back(3);
}

TEST(update_data, mixed_modifications_4) {
	CircularBufferFixed<int, 8> cb;
	cb.push_back(-7);
	int value = cb.pop_back();
	ASSERT_EQ(value, -7);
	ASSERT_EQ(cb.size(), 0u);
	value = cb.pop_back();
	ASSERT_EQ(cb.size(), 0u);
	cb.push_front(-6);
	ASSERT_EQ(cb.size(), 1u);
	value = cb.pop_back();
	ASSERT_EQ(cb.size(), 0u);
	ASSERT_EQ(value, -6);
	cb.push_back(1);
	ASSERT_EQ(cb.size(), 1u);
	value = cb.pop_front();
	ASSERT_EQ(cb.size(), 0u);
	ASSERT_EQ(value, 1);
	value = cb.pop_back();
	ASSERT_EQ(cb.size(), 0u);
	value = cb.pop_back();
	ASSERT_EQ(cb.size(), 0u);
}

TEST(access, method_at) {
	CircularBufferFixed<int,3 > cb (42);
	cb.push_back(1);
	cb.push_back(2);
	cb.push_back(3);
	ASSERT_EQ(cb.at(0), 1);
	ASSERT_EQ(cb.at(1), 2);
	ASSERT_EQ(cb.at(2), 3);
}

TEST(access, operator_square_brackets) {
	CircularBufferFixed<int, 3> cb (42);
	cb.push_back(1);
	cb.push_back(2);
	cb.push_back(3);
	ASSERT_EQ(cb[0], 1);
	ASSERT_EQ(cb[1], 2);
	ASSERT_EQ(cb[2], 3);
}

TEST(access, capacity) {
	CircularBufferFixed<int,3 > cb1 (42);
	ASSERT_EQ(cb1.capacity(), 3u);
	CircularBufferFixed<int, 2> cb2 (42);
	ASSERT_EQ(cb2.capacity(), 2u);
}

TEST(access, size) {
	CircularBufferFixed<int, 3> cb;
	ASSERT_EQ(cb.size(), 0u);
	cb.push_back(1);
	ASSERT_EQ(cb.size(), 1u);
	cb.push_back(2);
	ASSERT_EQ(cb.size(), 2u);
	cb.push_back(3);
	ASSERT_EQ(cb.size(), 3u);
	cb.push_back(4);
	ASSERT_EQ(cb.size(), 3u);

	cb.pop_front();
	ASSERT_EQ(cb.size(), 2u);
	cb.pop_front();
	ASSERT_EQ(cb.size(), 1u);
	cb.pop_front();
	ASSERT_EQ(cb.size(), 0u);
	cb.pop_front();
	ASSERT_EQ(cb.size(), 0u);

	cb.push_front(1);
	ASSERT_EQ(cb.size(), 1u);
	cb.push_front(2);
	ASSERT_EQ(cb.size(), 2u);
	cb.push_front(3);
	ASSERT_EQ(cb.size(), 3u);
	cb.push_front(4);
	ASSERT_EQ(cb.size(), 3u);

	cb.pop_back();
	ASSERT_EQ(cb.size(), 2u);
	cb.pop_back();
	ASSERT_EQ(cb.size(), 1u);
	cb.pop_back();
	ASSERT_EQ(cb.size(), 0u);
	cb.pop_back();
	ASSERT_EQ(cb.size(), 0u);
}

TEST(access, empty) {
	constexpr int capacity {3};
	CircularBufferFixed<int, capacity> cb;
	ASSERT_TRUE(cb.empty());
	cb.push_back(1);
	ASSERT_FALSE(cb.empty());
	cb.push_back(2);
	ASSERT_FALSE(cb.empty());
	cb.push_back(3);
	ASSERT_FALSE(cb.empty());
	cb.push_back(4);
	ASSERT_FALSE(cb.empty());

	for (int i = 0; i != capacity - 1; ++i){
		cb.pop_back();
		ASSERT_FALSE(cb.empty());
	}
	cb.pop_back();
	ASSERT_TRUE(cb.empty());
	cb.pop_back();
	ASSERT_TRUE(cb.empty());

	cb.push_front(1);
	ASSERT_FALSE(cb.empty());
	cb.push_front(2);
	ASSERT_FALSE(cb.empty());
	cb.push_front(3);
	ASSERT_FALSE(cb.empty());
	cb.push_front(4);
	ASSERT_FALSE(cb.empty());

	for (int i = 0; i != capacity - 1; ++i){
		cb.pop_front();
		ASSERT_FALSE(cb.empty());
	}
	cb.pop_front();
	ASSERT_TRUE(cb.empty());
	cb.pop_front();
	ASSERT_TRUE(cb.empty());
}

TEST(special_case, size_of_one) {
	CircularBufferFixed<int, 1> cb (42);
	cb.push_back(1);
	ASSERT_EQ(cb.front(), 1);
	ASSERT_EQ(cb.back(), 1);
	ASSERT_EQ(cb.at(0), 1);
	ASSERT_EQ(cb[0], 1);
	cb.push_back(2);
	ASSERT_EQ(cb.front(), 2);
	ASSERT_EQ(cb.back(), 2);
	ASSERT_EQ(cb.at(0), 2);
	ASSERT_EQ(cb[0], 2);
	cb.push_front(3);
	ASSERT_EQ(cb.front(), 3);
	ASSERT_EQ(cb.back(), 3);
	ASSERT_EQ(cb.at(0), 3);
	ASSERT_EQ(cb[0], 3);
}

TEST(iterating, using_at ) {
	CircularBufferFixed<int, 3> cb (42);
	std::vector<int> v {1, 2, 3};
	for (auto const elem : v) {
		cb.push_back(elem);
	}

	//logic state is {1, 2, 3}, real state is {1, 2, 3}
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb.at(i), v.at(i));
	}

	cb.push_back(0); //logic state is {2, 3, 0}, real state is {0, 2, 3}
	v = {2, 3, 0};
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb.at(i), v.at(i));
	}

	cb.pop_front();
	cb.push_back(1); //logic state is {3, 0, 1}, real state is {0, 1, 3}
	v = {3, 0, 1};
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb.at(i), v.at(i));
	}
}

TEST(iterating, using_sq_brackets ) {
	CircularBufferFixed<int, 3> cb (42);
	std::vector<int> v {1, 2, 3};
	for (auto const elem : v) {
		cb.push_back(elem);
	}

	//logic state is {1, 2, 3}, real state is {1, 2, 3}
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb[i], v[i]);
	}

	cb.push_back(0); //logic state is {2, 3, 0}, real state is {0, 2, 3}
	v = {2, 3, 0};
	for (std::size_t i = 0; i != cb.size(); ++i) {
		ASSERT_EQ(cb[i], v[i]);
	}

	cb.pop_front();
	cb.push_back(1); //logic state is {3, 0, 1}, real state is {0, 1, 3}
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
