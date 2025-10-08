//
// Created by moosm on 9/25/2025.
//

#include "log2word/log2word.hpp"

#include <fstream>
#include <iostream>

#include "log2word/solver/feedback.hpp"


#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <numeric>

#include "log2word/log2word.hpp"
#include "log2word/solver/feedback.hpp"

void test_wordle_index(log2word::core& c, size_t target_index) {
    if (target_index >= c.get_answer_list().size()) {
        std::cerr << "Invalid index!\n";
        return;
    }

    std::string_view correct = c.get_answer_list()[target_index];

    std::vector<size_t> possible_answers(c.get_answer_list().size());
    for (int i = 0; i < c.get_answer_list().size(); ++i)
    {
        possible_answers[i] = c.answer_to_word(i);
    }

    std::cout << "CORRECT: " << correct << "\n";

    for (int t = 0; t < 6; ++t)
    {
        size_t best_guess = c.get_word_scores_sorted(possible_answers,0)[0].index;
        log2word::solver::feedback fb(c.get_word(best_guess),correct);
        c.limit_guesses(best_guess,fb,possible_answers);

        std::cout << " guessed: " << c.get_word_list()[best_guess] << " remaining: " << possible_answers.size() << "; ";

        for (size_t j = 0; j < possible_answers.size(); ++j)
        {
            //std::cout << c.get_word(possible_answers[j]) << "\n";
        }

        if ((best_guess) == c.answer_to_word(target_index))
        {
            std::cout << "\n found answer: " << c.get_word_list()[best_guess] << " in " << t+1 << "\n";
            break;
        }
    }
    std::cout << std::endl;
}

int main()
{
    log2word::core c("../data/all.txt","../data/answers.txt");
    c.pre_calculate(true);

    {
        log2word::common::timing::scoped_timer t("### TEST WORDLE WORDS ###",true);
        for (size_t i = 0; i < 10; ++i)
        {
            test_wordle_index(c, i);
        }
    }
}