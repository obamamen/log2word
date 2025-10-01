//
// Created by moosm on 9/25/2025.
//

#include "log2word/log2word.hpp"

#include <fstream>
#include <iostream>

#include "log2word/solver/feedback.hpp"

// github push test

int main()
{
      log2word::core c("data/all.txt","data/answers.txt");
      c.pre_calculate(true);
      c.debug_output_entropy();
}
