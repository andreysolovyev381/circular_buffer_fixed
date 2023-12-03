### Reasoning behind
- I have a specific usage of data window, specifically, my data comes in one at a time, so I am mostly interested in comparing joint usage of the following functionality:
```cpp 
    push_back();
    pop_front();
```

- I tried to bench againt few commonly used containers, seems like there is chance to do slightly better (~5x times for my task). 
- Refer to the benchmarking details here: [bench](https://github.com/andreysolovyev381/circular_buffer_bench)

- Feel free to use at your own risk, no guarantees of any kind are given whatsoever. 