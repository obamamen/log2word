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
    log2word::core c("../data/all.txt","../data/answers.txt");
    c.pre_calculate(true);

    while (true)
    {
        auto gu = log2word::solver::game_user(c);
    }
}