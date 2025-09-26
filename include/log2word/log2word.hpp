//
// Created by moosm on 9/26/2025.
//


#ifndef SOLVER_HPP
#define SOLVER_HPP


#include <string>
#include <vector>
#include <iostream>
#include <fstream>


#include "common/io/logger.hpp"
#include "common/io/file_loader.hpp"


///
namespace log2word
{
    /// @class core
    /// @brief Class that handles everything to analyzing games and solving.
    class core
    {

    public: // Public members

    private:// Private members

        /// The list of all the words in the wordle list
        std::vector<std::string> all_words;

        /// The list of words that can we answers
        std::vector<std::string> answers;

    public: // Public functions

        /// @brief Constructs a solver instance.
        /// @param all_words_path Filepath of the complete word list (includes answers).
        /// @param answers_path [optional] Filepath of only the answers (can be empty for official ruling).
        /// @details @ref all_words
        explicit core(const std::string& all_words_path, const std::string& answers_path = "")
        {
            common::io::file_loader::load_word_file_to_list(all_words, all_words_path);

            common::io::file_loader::load_word_file_to_list(answers,
                (answers_path.empty()) ? all_words_path : answers_path);
        }

        ///
        ~core() = default;

        /// @param stream [optional]
        /// @param top [optional] shows the first 'top'
        void debug_output_lists(std::ostream& stream = std::cout, const int top = 4) const
        {
            std::cout << "All [size] = " << all_words.size() << '\n';
            common::io::logger::log_list(all_words, top, stream);
            std::cout << '\n';

            std::cout << "Answers [size] = " << answers.size() << '\n';
            common::io::logger::log_list(answers, top, stream);
        }

    private:// private functions

    };
}

#endif //SOLVER_HPP
