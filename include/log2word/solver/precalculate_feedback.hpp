// -----------------------------------------------------------------------------
//  
//  log2word - precalculate_feedback.hpp
//
// -----------------------------------------------------------------------------


#ifndef LOG2WORD_PRECALCULATE_FEEDBACK
#define LOG2WORD_PRECALCULATE_FEEDBACK

#include "feedback.hpp"
#include "log2word/common/threading/parallel_for.hpp"

namespace log2word::solver
{
    inline void compute_feedback_table(
        std::vector<std::vector<feedback>>& target_lut,
        const std::vector<std::string>& guess_list,
        const std::vector<std::string>& answer_list,
        const bool debug = false,
        std::ostream& stream = std::cout)
    {
        common::threading::parallel_for(guess_list.size(),
            [&target_lut, &answer_list, &guess_list](const size_t start, const size_t end) {
                for (size_t guess = start; guess < end; guess++)
                    for (size_t answer = 0; answer < answer_list.size(); answer++)
                        target_lut[guess].emplace_back(guess_list[guess], answer_list[answer]);
            }, true
        );
    }
}

#endif // LOG2WORD_PRECALCULATE_FEEDBACK