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
#include <random>

#include "log2word/log2word.hpp"
#include "log2word/solver/feedback.hpp"

int test_wordle_index(log2word::core& c, size_t target_index) {
    if (target_index >= c.get_answer_list().size()) {
        std::cerr << "Invalid index!\n";
        return -1;
    }

    std::string_view correct = c.get_answer_list()[target_index];

    std::vector<size_t> possible_answers(c.get_answer_list().size());
    for (int i = 0; i < c.get_answer_list().size(); ++i)
    {
        possible_answers[i] = c.answer_to_word(i);
    }

    //std::cout << "CORRECT: " << correct << "\n";

    for (int t = 0; t < 6; ++t)
    {
        size_t best_guess = c.get_word_scores_sorted(possible_answers,0)[0].index;
        log2word::solver::feedback fb(c.get_word(best_guess),correct);
        c.limit_guesses(best_guess,fb,possible_answers);

        //std::cout << " guessed: " << c.get_word_list()[best_guess] << " remaining: " << possible_answers.size() << "; ";

        for (size_t j = 0; j < possible_answers.size(); ++j)
        {
            //std::cout << c.get_word(possible_answers[j]) << "\n";
        }

        if ((best_guess) == c.answer_to_word(target_index))
        {
            //std::cout << "\n found answer: " << c.get_word_list()[best_guess] << " in " << t+1 << "\n";
            return t;
        }
    }
    std::cout << std::endl;
    return -1;
}

int main()
{
    size_t sample_size = 125;
    log2word::core c("../data/all.txt","../data/answers.txt");
    c.pre_calculate(true);

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, c.get_answer_list().size() - 1);

    {
        size_t count_found{};
        size_t total_tries{};
        size_t count_failed{};
        log2word::common::timing::scoped_timer t("### TEST WORDLE WORDS ###",true);

        std::array<int,6> distribution{};

        for (size_t i = 0; i < sample_size; ++i)
        {
            size_t index = dist(rng);
            int tr = test_wordle_index(c, index);
            if (tr >= 0)
            {
                ++count_found;
                total_tries += tr+1;
                distribution[tr]++;
            }
            else
            {
                ++count_failed;
            }
        }

        std::cout << "sample size: " << sample_size << "\n";
        std::cout << "solved: " << count_found << " | failed ): " << count_failed << "\n";
        if (count_found > 0)
        {
            const double avg = (double)total_tries / count_found;
            std::cout << "avg: " << avg << "\n";

            std::cout << "dist:\n";
            for (size_t i = 0; i < distribution.size(); ++i)
            {
                int count = distribution[i];
                double pct = 100.0 * count / count_found;
                std::cout << (i + 1) << ": " << count << " (" << pct << "%)\n";
            }
        }

    }
}