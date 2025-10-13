//
// Created by moosm on 10/9/2025.
//

#ifndef LOG2WORD_CORE_H
#define LOG2WORD_CORE_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <unordered_map>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <unordered_set>
#include <limits>


#include "../common/io/logger.hpp"
#include "../common/io/file_loader.hpp"
#include "../common/timing/scoped_timer.hpp"
#include "feedback.hpp"
#include "precalculate_feedback.hpp"


namespace log2word
{

    class core
    {
        struct word_score
        {
            double entropy{};
            double score{};

            [[nodiscard]] std::string to_string() const
            {
                return "[" + std::to_string(score) + "]"; // | " + std::to_string(entropy);
            }

            [[nodiscard]] static bool better(const word_score &a, const word_score &b)
            {
                return a.score > b.score;
            }
        };

        struct unordered_word_score
        {
            word_score word_score{};
            size_t index{};

            [[nodiscard]] static bool better(const unordered_word_score &a, const unordered_word_score &b)
            {
                return word_score::better(a.word_score, b.word_score);
            }
        };

    public:

    private:
        std::vector<std::string> all_words_list{};
        std::vector<std::string> answers_list{};

        /// Instead of precalculating between all->all + all->answers.
        /// We only need an all->all.
        /// We can have a LUT to map answers to the index in the all_words_list
        std::vector<size_t> answers{};

        std::vector<std::vector<feedback>> all_to_all_feedbackLUT{};

        std::vector<bool> all_is_in_answers{};

    public:

        explicit core(const std::string& all_words_path, const std::string& answers_path = "")
        {

            if (!common::io::load_word_file_to_list(all_words_list, all_words_path))
            {
                throw std::runtime_error("failed to load all_words_list");
            }

            if (!(common::io::load_word_file_to_list(get_answer_list(),
                (answers_path.empty()) ? all_words_path : answers_path)))
            {
                throw std::runtime_error("failed to load answer_list");
            }
        }

        core() = delete;

        ~core() = default;

        [[nodiscard]] size_t answer_to_word(const size_t answer) const { return answers[answer]; }

        [[nodiscard]] std::vector<size_t> get_answers_indices() const { return answers; }

        [[nodiscard]] std::vector<std::string>& get_answer_list() { return answers_list; }
        [[nodiscard]] const std::vector<std::string>& get_answer_list() const { return answers_list; }

        [[nodiscard]] std::vector<std::string>& get_word_list() { return all_words_list; }
        [[nodiscard]] const std::vector<std::string>& get_word_list() const { return all_words_list; }

        [[nodiscard]] std::string_view get_word(const size_t index) const { return get_word_list()[index]; }

        [[nodiscard]] std::string_view get_answer(const size_t index) const{ return get_answer_list()[index]; }

        void pre_calculate(const bool debug = false, std::ostream& stream = std::cout)
        {
            common::timing::scoped_timer t("### PRE CALCULATE FEEDBACK ###", debug, stream);

            std::unordered_map<std::string_view, size_t> word_to_index;
            const auto& wl = get_word_list();
            for (size_t i = 0; i < wl.size(); ++i)
            {
                word_to_index[wl[i]] = i;
            }
            const auto& al = get_answer_list();
            answers.resize(al.size());
            for (size_t i = 0; i < al.size(); ++i)
            {
                answers[i] = word_to_index[al[i]];
            }

            all_to_all_feedbackLUT.resize(all_words_list.size());

            for (size_t i = 0; i < all_words_list.size(); ++i)
            {
                all_to_all_feedbackLUT[i].resize(all_words_list.size());
            }

            {
                common::timing::scoped_timer _("### ALL TO ALL FEEDBACK ###", debug, stream);
                compute_feedback_table(all_to_all_feedbackLUT, all_words_list, all_words_list,debug);
            }

            {
                common::timing::scoped_timer _("### ALL IS IN ANSWERS ###", debug, stream);
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
                },debug);
            }

            if (debug)
            std::cout << "words=" << all_words_list.size()
                << " answers=" << answers.size()
                << " feedbackLUT=" << all_to_all_feedbackLUT.size() << std::endl;
        }

        void debug_output_lists(std::ostream& stream = std::cout, const int top = 4) const
        {
            std::cout << "All [size] = " << all_words_list.size() << '\n';
            common::io::log_list(this->get_word_list(), top, stream);
            std::cout << '\n';

            std::cout << "Answers [size] = " << answers.size() << '\n';
            common::io::log_list(this->get_answer_list(), top, stream);
        }

        void debug_output_entropy(std::ostream& stream = std::cout, const size_t top = 10)
        {
            common::timing::scoped_timer t("### CALCULATING ENTROPY ###", true);

            const auto scores = get_word_scores_sorted(answers,top);
            for (size_t i = 0; i < top; ++i)
            {
                stream << i << ". " << get_word_list()[scores[i].index] << " ; " << scores[i].word_score.to_string() <<'\n';
            }
        }

        [[nodiscard]] std::vector<unordered_word_score> get_word_scores_sorted(
            const std::vector<size_t>& possible_answers,
            const size_t top = 0) const
        {
            const auto scores = compute_scores(possible_answers);
            std::vector<unordered_word_score> sorted_scores(scores.size());
            for (size_t i = 0; i < scores.size(); ++i)
            {
                sorted_scores[i].word_score = scores[i];
                sorted_scores[i].index = i;
            }

            if (top == 0)
            {
                std::sort(
                    sorted_scores.begin(),
                    sorted_scores.end(),
                    unordered_word_score::better
                );
            } else
            {
                std::partial_sort(
                    sorted_scores.begin(),
                    sorted_scores.begin() + top,
                    sorted_scores.end(),
                    unordered_word_score::better
                );
            }

            if (possible_answers.size() == 1)
            {
                sorted_scores[0].index = possible_answers[0];
                sorted_scores[0].word_score.score = 1.0;
            }

            return sorted_scores;
        }

        [[nodiscard]] double compute_expected_future_entropy(
            const size_t guess,
            const std::vector<size_t>& possible_answers,
            const size_t sample_limit = 1
        ) const
        {
            const auto counts = compute_feedback_counts(guess, possible_answers);
            const size_t total = possible_answers.size();
            double expected_future_entropy = 0.0;

            for (size_t f = 0; f < counts.size(); ++f)
            {
                int c = counts[f];
                if (c <= 1) continue;
                double p = double(c) / double(total);

                double sub_entropy = 0.0;
                size_t seen = 0;

                for (const auto idx : possible_answers)
                {
                    if (all_to_all_feedbackLUT[guess][idx].get_bits() == f)
                    {
                        sub_entropy += compute_entropy(idx, possible_answers);
                        if (++seen >= sample_limit) break;
                    }
                }

                if (seen > 0)
                    expected_future_entropy += p * (sub_entropy / double(seen));
            }

            return expected_future_entropy;
        }

    public:
        [[nodiscard]] std::vector<word_score> compute_scores(
            const std::vector<size_t>& possible_answers) const
        {
            const size_t size = get_word_list().size();
            std::vector<word_score> scores(size);

            const bool only_answers = possible_answers.size() <= 24;
            const bool do_future_entropy = (possible_answers.size() <= 128);
            if (!only_answers)
            {
                common::threading::parallel_for(size,
                [&](const size_t start, const size_t end)
                {
                    for (size_t guess_idx = start; guess_idx < end; guess_idx++)
                    {
                        double H = compute_entropy(guess_idx, possible_answers);
                        double future_H = 0.0;

                        if (do_future_entropy)
                            future_H = compute_expected_future_entropy(guess_idx, possible_answers);

                        scores[guess_idx].entropy = H;
                        scores[guess_idx].score = H - 1 * future_H;
                    }
                });

                return scores;
            }
            else
            {
                for (const auto guess_idx : possible_answers)
                {
                    double H = compute_entropy(guess_idx, possible_answers);
                    double future_H = 0.0;

                    if (do_future_entropy)
                        future_H = compute_expected_future_entropy(guess_idx, possible_answers);

                    scores[guess_idx].entropy = H;
                    scores[guess_idx].score = H - 1 * future_H;
                }

                return scores;
            }
        }

        [[nodiscard]] word_score compute_score(
            const size_t index,
            const std::vector<size_t>& possible_answers) const
        {
            double H = compute_entropy(index, possible_answers);
            double future_H = 0.0;

            if (possible_answers.size() <= 24)
                future_H = compute_expected_future_entropy(index, possible_answers);

            word_score s;
            s.entropy = H;
            s.score = H - 1 * future_H;
            return s;
        }

        [[nodiscard]] double compute_entropy(const size_t guess, const std::vector<size_t>& possible_answers) const
        {
            const auto counts = compute_feedback_counts(guess, possible_answers);
            return compute_entropy_from_counts(counts, possible_answers.size());
        }

        void limit_guesses(const size_t guess, const feedback& feedback_received, std::vector<size_t>& possible_answers) const
        {
            const auto target_bits = feedback_received.get_bits();
            size_t write = 0;

            for (size_t read = 0; read < possible_answers.size(); ++read)
            {
                const size_t answer_idx = possible_answers[read];
                if (all_to_all_feedbackLUT[guess][answer_idx].get_bits() == target_bits)
                {
                    possible_answers[write++] = answer_idx;
                }
            }

            possible_answers.resize(write);
        }


    private:
        [[nodiscard]] std::array<int, 1 << (5 * 2)> compute_feedback_counts(
            const size_t guess,
            const std::vector<size_t>& possible_answers) const
        {
            constexpr size_t MAX_FEEDBACKS = 1 << (5 * 2);
            std::array<int, MAX_FEEDBACKS> counts{};

            for (const auto possible_answer : possible_answers)
            {
                ++counts[all_to_all_feedbackLUT[guess][possible_answer].get_bits()];
            }

            return counts;
        }

        [[nodiscard]] static double compute_entropy_from_counts(
            const std::array<int, 1 << (5 * 2)>& counts,
            const size_t total)
        {
            double entropy = 0.0;
            const double inv_total = 1.0 / (double)total;

            for (const int c : counts)
            {
                if (c == 0) continue;
                const double p = c * inv_total;
                entropy -= p * std::log2(p);
            }

            return entropy;
        }

        [[nodiscard]] static double compute_expected_log_remaining(
            const std::array<int, 1 << (5 * 2)>& counts,
            const size_t total,
            const bool is_answer)
        {
            double expected = 0.0;
            const double inv_total = 1.0 / (double)total;

            for (const int c : counts)
            {
                if (c == 0) continue;
                const double p = c * inv_total;

                const int remaining = is_answer ? std::max(0, c - 1) : c;
                if (remaining > 0)
                {
                    expected += p * std::log2(static_cast<double>(remaining));
                }
            }

            return expected;
        }

    };
}

#endif //LOG2WORD_CORE_H