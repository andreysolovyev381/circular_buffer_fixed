### Reasoning behind
- I have a specific usage of data window, specifically, my data comes in one at a time, so I am mostly interested in comparing joint usage of the following functionality:
```cpp 
    push_back();
    pop_front();
```

- I tried to bench against few commonly used containers, seems like there is chance to do slightly better (~5x times for my task ON MY MACHINE). 
- Refer to the benchmarking details here: [bench](https://github.com/andreysolovyev381/circular_buffer_bench)
- See usage examples in the tests.cpp file.
- Important thing is that this impl doesn't call a dtor of an element whee popping, therefore it should do less work that other implementations, ie boost::circular_buffer. But it becomes developer's problem to track data reliability.

- **Feel free to use at your own risk, no guarantees of any kind are given whatsoever. MIT license, see attached.** 

### Update on 30th of March 2024
- Added pushFront and popBack methods