//
// Created by moosm on 10/9/2025.
//

#ifndef LOG2WORD_SOLVER_HPP
#define LOG2WORD_SOLVER_HPP


#include <limits>
#include <random>


#include "../core/core.hpp"


#define MAX_TRIES 6


namespace log2word::solver
{
    struct game_result
    {
    private:
        const int tries_count = -1;
    public:
        explicit game_result(const int tries) : tries_count(tries) {}
        game_result() = default;

        bool win() const { return tries_count > 0; }
        [[nodiscard]] auto tries() const { return tries_count; }
    };

    inline log2word::feedback get_user_feedback()
    {
        std::string input;
        while (true)
        {
            std::cout << "input feedback <G,Y,X> (eg GYXYX): ";
            std::getline(std::cin, input);
            if (input.size() == 5)
                break;
            std::cout << "incorrect input, try again\n";
        }

        log2word::feedback fb{};
        for (int i = 0; i < 5; i++)
            fb.set(i, feedback::feedback_type_from_char(std::tolower(input[i])));

        return fb;
    }

    inline size_t get_user_guess(const core& core)
    {
        std::string input;
        while (true)
        {
            std::cout << "input guess: ";
            std::getline(std::cin, input);
            if (input.size() == 5)
            {
                auto itter = std::find_if(core.get_word_list().begin(), core.get_word_list().end(), [input](const std::string& w)
                {
                    return input == w;
                });

                if (itter == core.get_word_list().end())
                {
                    std::cout << "unguessable word, try again\n";
                    continue;
                }

                return itter - core.get_word_list().begin();
            }
            std::cout << "incorrect input length, try again\n";
        }
    }

    inline game_result game_simulate(const core& core, const size_t target = ULLONG_MAX)
    {
        std::vector<size_t> possible_answers = core.get_answers_indices();

        size_t target_in_all = core.answer_to_word(target);

        for (int t = 0; t < MAX_TRIES; ++t)
        {
            size_t best_guess = core.get_word_scores_sorted(possible_answers,1)[0].index;

            log2word::feedback fb(core.get_word(best_guess),core.get_word(target_in_all));
            core.limit_guesses(best_guess,fb,possible_answers);

            if ((best_guess) == core.answer_to_word(target))
            {
                return game_result{t};
            }
        }

        return {};
    }

    inline game_result game_user(core& core, const size_t top = 10)
    {
        std::vector<size_t> possible_answers = core.get_answers_indices();

        for (int t = 0; t < MAX_TRIES; ++t)
        {
            common::timing::scoped_timer st{};
            auto top_list = core.get_word_scores_sorted(possible_answers);
            std::cout << "(found in " << st.ms() << " ms)" << std::endl;
            std::cout << "Possible answers: " << possible_answers.size() << std::endl;
            for (int i = 1; i <= std::min(top,top_list.size()); ++i)
            {
                std::cout << "#" << i << " " << core.get_word(top_list[i-1].index) << " " << top_list[i-1].word_score.to_string() << "\n";
            }
            size_t best_guess = get_user_guess(core);

            log2word::feedback fb = get_user_feedback();
            core.limit_guesses(best_guess,fb,possible_answers);

            if (fb())
            {
                return game_result{t};
            }
        }

        return {};
    }
}

#endif //LOG2WORD_SOLVER_HPP