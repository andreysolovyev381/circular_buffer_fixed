### Reasoning behind
- I have a specific usage of data window, specifically, my data comes in one at a time, so I am mostly interested in comparing joint usage of the following functionality:
```cpp 
    push_back();
    pop_front();
```
Naturally, there are some solutions for that, but the idea is to use `std::array` as it is expected to win over known implementations.  

### WIP problem
Something strange happens here - circular buffer on `std::array` w/o any reallocation and w/o calling an elem dtor upon destruction loses to both `std::deque` and `boost::circular_buffer`. Trying to figure out what are the reasons.
```bash
-----------------------------------------------------------------------
Benchmark                             Time             CPU   Iterations
-----------------------------------------------------------------------
PushBackPopFrontDeque/64            140 ns          139 ns      5045115
PushBackPopFrontDeque/512          1072 ns         1070 ns       660096
PushBackPopFrontDeque/2048         4227 ns         4219 ns       165916
PushBackPopFrontBoostCB/64         88.5 ns         88.4 ns      7919658
PushBackPopFrontBoostCB/512         691 ns          690 ns      1014275
PushBackPopFrontBoostCB/2048       2759 ns         2754 ns       254187
PushBackPopFrontCBFixed/64          175 ns          174 ns      4013473
PushBackPopFrontCBFixed/512         856 ns          855 ns       818852
PushBackPopFrontCBFixed/2048       3406 ns         3400 ns       205866

```