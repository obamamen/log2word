// -----------------------------------------------------------------------------
//  
//  log2word - parallel_for.hpp
//
// -----------------------------------------------------------------------------


#ifndef LOG2WORD_PARALLEL_FOR
#define LOG2WORD_PARALLEL_FOR

#include <thread>
#include <vector>
#include <functional>
#include <algorithm>

namespace log2word::common::threading
{
    inline void parallel_for(const size_t total_items,
                             const std::function<void(size_t start, size_t end)>& work_func,
                             const bool debug = false,
                             std::ostream& stream = std::cout,
                             size_t num_threads = std::thread::hardware_concurrency())
    {
        if (total_items == 0) return;
        if (num_threads == 0) num_threads = 1;

        const size_t chunk_size = (total_items + num_threads - 1) / num_threads;
        std::vector<std::thread> threads;

        for (size_t t = 0; t < num_threads; ++t)
        {
            size_t start = t * chunk_size;
            size_t end = std::min(start + chunk_size, total_items);

            if (debug) stream << "Thread " << t+1 << " starting at " << start << std::endl;

            if (start < end)
            {
                threads.emplace_back([start, end, work_func]() {
                    work_func(start, end);
                });
            }
        }

        for (auto& thread : threads)
            thread.join();
    }
}

#endif // LOG2WORD_PARALLEL_FOR