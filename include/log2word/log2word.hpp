//
// Created by moosm on 9/26/2025.
//


#ifndef SOLVER_HPP
#define SOLVER_HPP


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <unordered_map>
#include <algorithm>
#include <cmath>


#include "common/io/logger.hpp"
#include "common/io/file_loader.hpp"
#include "common/timing/scoped_timer.hpp"
#include "solver/feedback.hpp"
#include "solver/precalculate_feedback.hpp"


namespace log2word
{

    class core
    {

    public:

    public: // should be private, use public for easy debugging

        std::vector<std::string> all_words;
        std::vector<std::string> answers;

        std::vector<std::vector<solver::feedback>> all_to_all_feedbackLUT{};
        std::vector<std::vector<solver::feedback>> all_to_answer_feedbackLUT{};

    public:

        explicit core(const std::string& all_words_path, const std::string& answers_path = "")
        {
            common::io::load_word_file_to_list(all_words, all_words_path);

            common::io::load_word_file_to_list(answers,
                (answers_path.empty()) ? all_words_path : answers_path);
        }

        ~core() = default;

        void pre_calculate(const bool debug = false, std::ostream& stream = std::cout)
        {
            common::timing::scoped_timer t("### PRE CALCULATE FEEDBACK ###", debug);

            all_to_all_feedbackLUT.resize(all_words.size());
            all_to_answer_feedbackLUT.resize(all_words.size());

            for (size_t i = 0; i < all_words.size(); ++i)
            {
                all_to_all_feedbackLUT[i].reserve(all_words.size());
                all_to_answer_feedbackLUT[i].reserve(answers.size());
            }

            {
                common::timing::scoped_timer t("### ALL TO ALL FEEDBACK ###", debug);
                solver::compute_feedback_table(all_to_all_feedbackLUT, all_words, all_words,true);
            }

            {
                common::timing::scoped_timer t("### ALL TO ANSWER FEEDBACK ###", debug);
                solver::compute_feedback_table(all_to_answer_feedbackLUT,all_words,answers,true);
            }
        }

        void debug_output_lists(std::ostream& stream = std::cout, const int top = 4) const
        {
            std::cout << "All [size] = " << all_words.size() << '\n';
            common::io::log_list(all_words, top, stream);
            std::cout << '\n';

            std::cout << "Answers [size] = " << answers.size() << '\n';
            common::io::log_list(answers, top, stream);
        }

        void debug_output_entropy(std::ostream& stream = std::cout, const int top = 10)
        {
            common::timing::scoped_timer t("### CALCULATING ENTROPY ###", true);

            std::vector<std::pair<std::string, double>> word_entropy;
            word_entropy.resize(all_words.size());

            common::threading::parallel_for(all_words.size(),
                [&](const size_t start, const size_t end)
                {
                    for (size_t guess_idx = start; guess_idx < end; guess_idx++)
                    {
                        double entropy = compute_entropy(all_to_answer_feedbackLUT[guess_idx], answers.size());
                        word_entropy[guess_idx] = {all_words[guess_idx], entropy };
                    }
                },
                false
            );


            const int n = std::min(top, static_cast<int>(word_entropy.size()));
            std::partial_sort(word_entropy.begin(),
                              word_entropy.begin() + n,
                              word_entropy.end(),
                              [](const auto& a, const auto& b) { return a.second > b.second; });

            stream << "\n=== TOP " << top << " WORDS BY SHANNON ENTROPY ===\n";
            stream << std::fixed << std::setprecision(4);

            for (int i = 0; i < n; i++)
            {
                stream << (i + 1) << ". " << word_entropy[i].first
                       << " : " << word_entropy[i].second << " bits\n";
            }

            stream << "\nBest starting word: " << word_entropy[0].first
                   << " (entropy: " << word_entropy[0].second << " bits)\n";
        }

        static double compute_entropy(const std::vector<solver::feedback>& feedbacks, const size_t total_answers)
        {
            constexpr size_t MAX_FEEDBACKS = 1 << (5 * 2);
            std::array<int, MAX_FEEDBACKS> counts{};

            for (const auto& fb : feedbacks)
                ++counts[fb.get_bits()];

            double entropy = 0.0;
            const auto total = static_cast<double>(total_answers);

            const double inv_total = 1.0 / total;
            for (const int c : counts)
            {
                if (c == 0) continue;
                const double p = c * inv_total;
                entropy -= p * std::log2(p);
            }

            return entropy;
        }

    private:

    };
}

#endif //SOLVER_HPP
