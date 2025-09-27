//
// Created by moosm on 9/26/2025.
//


#ifndef SOLVER_HPP
#define SOLVER_HPP


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>


#include "common/io/logger.hpp"
#include "common/io/file_loader.hpp"
#include "solver/feedback.hpp"
#include "solver/precalculate_feedback.hpp"


namespace log2word
{

    class core
    {

    public:

    public:

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
            if (debug) stream << "### PRE CALCULATE ###\n";

            all_to_all_feedbackLUT.resize(all_words.size());
            all_to_answer_feedbackLUT.resize(all_words.size());

            for (size_t i = 0; i < all_words.size(); ++i)
            {
                all_to_all_feedbackLUT[i].reserve(all_words.size());
                all_to_answer_feedbackLUT[i].reserve(answers.size());
            }

            if (debug) stream << "### all_to_all_feedback ###\n";
            solver::compute_feedback_table(all_to_all_feedbackLUT, all_words, all_words,true);
            if (debug) stream << "### all_to_answer_feedback ###\n";
            solver::compute_feedback_table(all_to_answer_feedbackLUT,all_words,answers,true);

            if (debug) stream << "### FINISHED ###\n";
        }

        void debug_output_lists(std::ostream& stream = std::cout, const int top = 4) const
        {
            std::cout << "All [size] = " << all_words.size() << '\n';
            common::io::log_list(all_words, top, stream);
            std::cout << '\n';

            std::cout << "Answers [size] = " << answers.size() << '\n';
            common::io::log_list(answers, top, stream);
        }

    private:

    };
}

#endif //SOLVER_HPP
