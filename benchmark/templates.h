//
// Created by Andrey Solovyev on 06/11/2021.
//

#pragma once

#include "randomer.h"

#include <vector>
#include <deque>
#include "boost/circular_buffer.hpp"
#include "../include/circular_buffer_fixed.hpp"

#include <type_traits>
#include <cassert>

using data_alias = double;

static constexpr data_alias lower_bound = -1.e5;
static constexpr data_alias upper_bound = 1.e5;
static constexpr data_alias  double_value {42.5};

static constexpr int cb_capacity {1 << 5};
using CBFixed = containers::CircularBufferFixed<data_alias, cb_capacity>;

template<typename Container>
requires (requires {!std::same_as<Container, CBFixed>;})
Container create (std::size_t size) {
	Container container;
	container.resize(size);
	return container;
}

template<>
CBFixed create (std::size_t) {
	CBFixed container(data_alias{});
	return container;
}

template<typename  Container>
Container copy (const Container &container) {
	return Container{begin(container), end(container)};
}

template<typename  Container>
Container address_at (Container const &container, std::size_t size) {
	Container results {create<Container>(size)};
	int j {0};
	for (std::size_t i = 0; i != size; ++i) {
		j = prm(0, size - 1);
		results.at(i) = container.at(j);
	}
	return results;
}

template<typename  Container>
Container address_square_brackets (Container const &container, std::size_t size) {
	Container results {create<Container>(size)};
	int j {0};
	for (std::size_t i = 0; i != size; ++i) {
		j = prm(0, size - 1);
		results[i] = container[j];
	}
	return results;
}

template<typename  Container>
Container push_back_on_empty (std::size_t new_elem_count) {
	Container container;
	for (std::size_t i = 0, j = new_elem_count; i != j; ++i) {
		container.push_back(double_value);
	}
	return container;
}


template<typename  Container>
int pop_front (Container &container) {
	int count = 0;
	assert(container.size() > 0);
	if constexpr (not std::is_same_v<Container, std::vector<typename Container::value_type>>){
		while (not container.empty()) {
			container.pop_front();
			++count;
		}
	}
	else{
		while (not container.empty()) {
			container.erase(container.begin());
			++count;
		}
	}
	return count;
}


template<typename  Container>
int push_back_then_pop_front (Container &container, std::size_t iterations) {
	assert(container.size() > 0);
	int count {0};
	for (std::size_t i = 0; i != iterations; ++i) {
		if constexpr (std::is_same_v<Container, std::vector<typename Container::value_type>>) {
			container.push_back(double_value);
			container.erase(container.begin());
		}
		else if constexpr (std::is_same_v<Container, std::deque<typename Container::value_type>>) {
			container.push_back(double_value);
			container.pop_front();
		}
		else if constexpr (std::is_same_v<Container, boost::circular_buffer<typename Container::value_type>>) {
			container.push_back(double_value);
		}
		else if constexpr (std::is_same_v<Container, containers::CircularBufferFixed<typename Container::value_type, Container::capacity()>>) {
			container.push_back(double_value);
		}
		count += count / double_value;
	}
	return count;
}
