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
#include <unordered_set>


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

    private:
        std::vector<std::string> all_words_list{};
        std::vector<std::string> answers_list{};

        /// Instead of precalculating between all->all + all->answers.
        /// We only need an all->all.
        /// We can have a LUT to map answers to the index in the all_words_list
        std::vector<size_t> answers{};

        std::vector<std::vector<solver::feedback>> all_to_all_feedbackLUT{};

        std::vector<bool> all_is_in_answers{};

    public:

        explicit core(const std::string& all_words_path, const std::string& answers_path = "")
        {
            common::io::load_word_file_to_list(all_words_list, all_words_path);

            common::io::load_word_file_to_list(get_answer_list(),
                (answers_path.empty()) ? all_words_path : answers_path);

            std::unordered_map<std::string_view, size_t> word_to_index;
            const auto& wl = get_word_list();
            for (size_t i = 0; i < wl.size(); ++i) { word_to_index[wl[i]] = i; }
            const auto& al = get_answer_list();
            answers.resize(al.size());
            for (size_t i = 0; i < al.size(); ++i) { answers[i] = word_to_index[al[i]]; }
        }

        ~core() = default;

        [[nodiscard]] std::vector<std::string>& get_answer_list() { return answers_list; }
        [[nodiscard]] const std::vector<std::string>& get_answer_list() const { return answers_list; }

        [[nodiscard]] std::vector<std::string>& get_word_list() { return all_words_list; }
        [[nodiscard]] const std::vector<std::string>& get_word_list() const { return all_words_list; }

        [[nodiscard]] std::string_view get_word(const size_t index) const { return get_word_list()[index]; }

        [[nodiscard]] std::string_view get_answer(const size_t index) const{ return get_answer_list()[index]; }

        void pre_calculate(const bool debug = false, std::ostream& stream = std::cout)
        {
            common::timing::scoped_timer t("### PRE CALCULATE FEEDBACK ###", debug);

            all_to_all_feedbackLUT.resize(all_words_list.size());

            for (size_t i = 0; i < all_words_list.size(); ++i)
            {
                all_to_all_feedbackLUT[i].reserve(all_words_list.size());
            }

            {
                common::timing::scoped_timer t("### ALL TO ALL FEEDBACK ###", debug);
                solver::compute_feedback_table(all_to_all_feedbackLUT, all_words_list, all_words_list,true);
            }

            {
                common::timing::scoped_timer t("### ALL IS IN ANSWERS ###", debug);
                all_is_in_answers.resize(all_words_list.size());
                const std::unordered_set answer_set(answers_list.begin(), answers_list.end());

                all_is_in_answers.resize(all_words_list.size());

                common::threading::parallel_for(all_words_list.size(),
                [&](const size_t start, const size_t end)
                {
                    for (size_t i = start; i < end; ++i)
                    {
                        all_is_in_answers[i] = answer_set.count(all_words_list[i]) > 0;
                    }
                });
            }
        }

        void debug_output_lists(std::ostream& stream = std::cout, const int top = 4) const
        {
            std::cout << "All [size] = " << all_words_list.size() << '\n';
            common::io::log_list(this->get_word_list(), top, stream);
            std::cout << '\n';

            std::cout << "Answers [size] = " << answers.size() << '\n';
            common::io::log_list(this->get_answer_list(), top, stream);
        }

        void debug_output_entropy(std::ostream& stream = std::cout, const int top = 10)
        {
            common::timing::scoped_timer t("### CALCULATING ENTROPY ###", true);

            std::vector<std::pair<std::string, double>> word_entropy;
            word_entropy.resize(all_words_list.size());

            common::threading::parallel_for(all_words_list.size(),
                [&](const size_t start, const size_t end)
                {
                    for (size_t guess_idx = start; guess_idx < end; guess_idx++)
                    {
                        double entropy = compute_entropy(guess_idx, answers.size());
                        word_entropy[guess_idx] = {all_words_list[guess_idx], entropy };
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

        [[nodiscard]] double compute_entropy(const size_t guess, const size_t total_answers) const
        {
            const size_t size = this->answers.size();

            constexpr size_t MAX_FEEDBACKS = 1 << (5 * 2);
            std::array<int, MAX_FEEDBACKS> counts{};

            for (size_t i = 0; i < size; ++i)
            {
                ++counts[all_to_all_feedbackLUT[guess][answers[i]].get_bits()];
            }


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
