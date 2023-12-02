//
// Created by Andrey Solovyev on 27/03/2023.
//

#include <gtest/gtest.h>
#include "circular_buffer_fixed.hpp"

using namespace culib;

TEST(CB_Fixed, Creation) {
	circular_buffer_fixed_t<int> cb1 (5);
	circular_buffer_fixed_t<int> cb2 (5, 42);
	ASSERT_EQ(cb2.front(), 42);
	ASSERT_EQ(cb2.back(), 42);

}

TEST(BasicContainer, Modification) {
	ASSERT_TRUE(false);
}

TEST(BasicContainer, Copy) {
	ASSERT_TRUE(false);
}

TEST(BasicContainer, Move) {
	ASSERT_TRUE(false);
}