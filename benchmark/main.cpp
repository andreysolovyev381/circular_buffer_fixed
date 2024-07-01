#include <benchmark/benchmark.h>
#include "templates.h"


int constexpr range_start {64};
int constexpr range_stop {1<<17};

static void WarmUp (benchmark::State& state) {
	[[maybe_unused]] int i {0};
	for (auto _ : state)
		i = warm_up();
}
BENCHMARK(WarmUp);


//-------------------------
// push_back then pop_front

static void PushBackPopFrontVector(benchmark::State& state) {
	auto v = create<std::vector<T>> (state.range(0));
	for (auto _ : state){
		push_back_then_pop_front<std::vector<T>> (v, state.range(0));
	}
}
BENCHMARK(PushBackPopFrontVector)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void PushBackPopFrontDeque(benchmark::State& state) {
	auto v = create<std::deque<T>> (state.range(0));
	for (auto _ : state){
		push_back_then_pop_front<std::deque<T>> (v, state.range(0));
	}
}
BENCHMARK(PushBackPopFrontDeque)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void PushBackPopFrontCB(benchmark::State& state) {
	auto v = create<boost::circular_buffer<T>> (state.range(0));
	for (auto _ : state){
		push_back_then_pop_front<boost::circular_buffer<T>> (v, state.range(0));
	}
}
BENCHMARK(PushBackPopFrontCB)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void PushBackPopFrontCBFixed(benchmark::State& state) {
	auto v = create<containers::CircularBufferFixed<T>> (state.range(0));
	for (auto _ : state){
		push_back_then_pop_front<containers::CircularBufferFixed<T>> (v, state.range(0));
	}
}
BENCHMARK(PushBackPopFrontCBFixed)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

//-------------------------



//-------------------------
// create containter

static void CreateVector(benchmark::State& state) {
	for (auto _ : state){
		auto v = create<std::vector<T>> (state.range(0));
		benchmark::DoNotOptimize(v);
	}
}
BENCHMARK(CreateVector)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void CreateDeque(benchmark::State& state) {
	for (auto _ : state){
		auto v = create<std::deque<T>> (state.range(0));
		benchmark::DoNotOptimize(v);
	}
}
BENCHMARK(CreateDeque)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void CreateCB(benchmark::State& state) {
	for (auto _ : state){
		auto v = create<boost::circular_buffer<T>> (state.range(0));
		benchmark::DoNotOptimize(v);
	}
}
BENCHMARK(CreateCB)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);
//-------------------------




//-------------------------
// copy containter

static void CopyVector(benchmark::State& state) {
	auto v = create<std::vector<T>> (state.range(0));
	for (auto _ : state){
		auto v_copy = copy<std::vector<T>> (v);
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(CopyVector)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void CopyDeque(benchmark::State& state) {
	auto v = create<std::deque<T>> (state.range(0));
	for (auto _ : state){
		auto v_copy = copy<std::deque<T>> (v);
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(CopyDeque)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void CopyCB(benchmark::State& state) {
	auto v = create<boost::circular_buffer<T>> (state.range(0));
	for (auto _ : state){
		auto v_copy = copy<boost::circular_buffer<T>> (v);
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(CopyCB)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);
//-------------------------




//-------------------------
// access element using .at()

static void AtVector(benchmark::State& state) {
	auto v = create<std::vector<T>> (state.range(0));
	for (auto _ : state){
		auto v_copy = address_at<std::vector<T>> (v, state.range(0));
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(AtVector)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void AtDeque(benchmark::State& state) {
	auto v = create<std::deque<T>> (state.range(0));
	for (auto _ : state){
		auto v_copy = address_at<std::deque<T>> (v, state.range(0));
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(AtDeque)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void AtCB(benchmark::State& state) {
	auto v = create<boost::circular_buffer<T>> (state.range(0));
	for (auto _ : state){
		auto v_copy = address_at<boost::circular_buffer<T>> (v, state.range(0));
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(AtCB)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);
//-------------------------




//-------------------------
// access element using []

static void BracketsVector(benchmark::State& state) {
	auto v = create<std::vector<T>> (state.range(0));
	for (auto _ : state){
		auto v_copy = address_square_brackets<std::vector<T>> (v, state.range(0));
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(BracketsVector)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void BracketsDeque(benchmark::State& state) {
	auto v = create<std::deque<T>> (state.range(0));
	for (auto _ : state){
		auto v_copy = address_square_brackets<std::deque<T>> (v, state.range(0));
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(BracketsDeque)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void BracketsCB(benchmark::State& state) {
	auto v = create<boost::circular_buffer<T>> (state.range(0));
	for (auto _ : state){
		auto v_copy = address_square_brackets<boost::circular_buffer<T>> (v, state.range(0));
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(BracketsCB)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);
//-------------------------



//-------------------------
// add element to the end

static void PushBackOnEmptyVector(benchmark::State& state) {
	for (auto _ : state){
		auto v_copy = push_back_on_empty<std::vector<T>> (state.range(0));
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(PushBackOnEmptyVector)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void PushBackOnEmptyDeque(benchmark::State& state) {
	for (auto _ : state){
		auto v_copy = push_back_on_empty<std::deque<T>> (state.range(0));
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(PushBackOnEmptyDeque)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void PushBackOnEmptyCB(benchmark::State& state) {
	for (auto _ : state){
		auto v_copy = push_back_on_empty<boost::circular_buffer<T>> (state.range(0));
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(PushBackOnEmptyCB)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);
//-------------------------



//-------------------------
// pop first elemennt

static void PopFrontVector(benchmark::State& state) {
	for (auto _ : state){
		auto v = create<std::vector<T>> (state.range(0));
		auto v_copy = pop_front<std::vector<T>> (v);
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(PopFrontVector)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void PopFrontDeque(benchmark::State& state) {
	for (auto _ : state){
		auto v = create<std::deque<T>> (state.range(0));
		auto v_copy = pop_front<std::deque<T>> (v);
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(PopFrontDeque)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);

static void PopFrontCB(benchmark::State& state) {
	for (auto _ : state){
		auto v = create<boost::circular_buffer<T>> (state.range(0));
		auto v_copy = pop_front<boost::circular_buffer<T>> (v);
		benchmark::DoNotOptimize(v_copy);
	}
}
BENCHMARK(PopFrontCB)->Range(range_start, range_stop)->Unit(benchmark::kNanosecond);
//-------------------------






BENCHMARK_MAIN();
