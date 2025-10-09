// -----------------------------------------------------------------------------
//  
//  log2word - precalculate_feedback.hpp
//
// -----------------------------------------------------------------------------


#ifndef LOG2WORD_PRECALCULATE_FEEDBACK
#define LOG2WORD_PRECALCULATE_FEEDBACK

#include "feedback.hpp"
#include "log2word/common/threading/parallel_for.hpp"

namespace log2word
{
    inline void compute_feedback_table(
        std::vector<std::vector<feedback>>& target_lut,
        const std::vector<std::string>& guess_list,
        const std::vector<std::string>& answer_list,
        const bool debug = false,
        std::ostream& stream = std::cout)
    {
        const size_t answer_list_size = answer_list.size();
        const size_t guess_list_size = guess_list.size();

        target_lut.reserve(guess_list_size);
        target_lut.resize(guess_list_size);

        common::threading::parallel_for(guess_list_size,
            [&target_lut, answer_list_size](const size_t start, const size_t end)
            {
                for (size_t i = start; i < end; i++)
                {
                    target_lut[i].resize(answer_list_size);
                }
            }, false, stream
        );

        common::threading::parallel_for(guess_list.size(),
            [&target_lut, &answer_list, &guess_list, answer_list_size](const size_t start, const size_t end)
            {
                for (size_t guess = start; guess < end; guess++)
                {
                    const std::string& guess_word = guess_list[guess];
                    auto& feedback_row = target_lut[guess];

                    for (size_t answer = 0; answer < answer_list_size; answer++)
                    {
                        feedback_row[answer] = feedback(guess_word, answer_list[answer]);
                    }
                }
            },
            debug, stream
        );
    }
}

#endif // LOG2WORD_PRECALCULATE_FEEDBACK