//
// Created by moosm on 9/26/2025.
//

#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "log2word__project.hpp"

#include "solver/logger.hpp"

namespace LOG2WORD_NAMESPACE
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
        explicit core(const std::string& all_words_path, const std::string& answers_path = std::string())
        {
            load_word_file_to_list(all_words, all_words_path);

            load_word_file_to_list(answers,
                (answers_path.empty()) ? all_words_path : answers_path);
        }


        /// @brief Destructor
        ~core() = default;


        /// @param stream [optional]
        /// @param top [optional] shows the first 'top'
        void debug_output_lists(std::ostream& stream = std::cout, const int top = 4) const
        {
            stream << __func__ << '\n';
            std::cout << "All [size] = " << all_words.size() << '\n';
            logger::log_list(stream, all_words, top);
            std::cout << "Answers [size] = " << answers.size() << '\n';
            logger::log_list(stream, answers, top);
        }

    private:// private functions

        /* PRIVATE */ static bool load_word_file_to_list
        (std::vector<std::string>& list, const std::string& path)
        {
            std::fstream file(path);
            if (!file.is_open()) return false;

            std::string line;
            while (true)
            {
                std::getline(file, line);

                if (file.fail()) return false;
                if (file.eof()) break;
                if (line.empty()) break;

                if (line[0] == '#') continue;

                list.push_back(line);
            }
            return true;
        }
    };
}

#endif //SOLVER_HPP
