//
// Created by moosm on 9/25/2025.
//

#include "log2word/log2word.hpp"

#include <fstream>
#include <iostream>

#include "../include/log2word/core/feedback.hpp"


#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <numeric>
#include <random>

#include "log2word/log2word.hpp"
#include "../include/log2word/core/feedback.hpp"
#include "log2word/solver/solver.hpp"

int main()
{
    log2word::core c("data/all.txt","data/answers.txt");
    {
        log2word::common::timing::scoped_timer st{};
        c.pre_calculate(false);
        std::cout << "(precalculated in " << st.ms() << " ms)\n" << std::endl;
    }

    // while (true)
    // {
    //     auto gu = log2word::solver::game_user(c);
    // }

    //for (int i = 0; i < c.get_word_list().size(); i++)
    //{
        std::cout << c.get_answer_list()[343]<<std::endl;
    {
        log2word::common::timing::scoped_timer _t("####",true);
        auto g = log2word::solver::game_simulate(c,434);
        std::cout << " : "<< g.tries() << std::endl;
    }
    //}
}