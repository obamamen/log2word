// -----------------------------------------------------------------------------
//  
//  log2word - scoped_timer.hpp
//
// -----------------------------------------------------------------------------


#ifndef LOG2WORD_SCOPED_TIMER
#define LOG2WORD_SCOPED_TIMER

#include <chrono>
#include <iostream>
#include <ostream>
#include <string>

namespace log2word::common::timing
{
    class scoped_timer
    {
    private:
        std::string name;
        std::chrono::high_resolution_clock::time_point start_time;
        std::ostream& stream = std::cout;
        bool log = true;

    public:
        explicit scoped_timer(std::string name, const bool do_log = false,std::ostream& stream = std::cout)
            : name(std::move(name)), stream(stream), log(do_log)
        {
            start_time = std::chrono::high_resolution_clock::now();
            if (log)
            {
                stream << "[" << this->name << "] started" << std::endl;
            }
        }

        scoped_timer(const scoped_timer&) = delete;
        scoped_timer(scoped_timer&&) = delete;
        scoped_timer& operator=(const scoped_timer&) = delete;
        scoped_timer& operator=(scoped_timer&&) = delete;

        ~scoped_timer()
        {
            if (log)
            {
                const auto end_time = std::chrono::high_resolution_clock::now();
                const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                stream << "[" << name << "] completed in " << duration.count() << " ms" << std::endl;
            }
        }

        void do_log(const bool do_log = false)
        {
            log = do_log;
        }
    };
}

#endif // LOG2WORD_SCOPED_TIMER