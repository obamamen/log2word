//
// Created by moosm on 9/25/2025.
//

#include "log2word/log2word.hpp"

#include <fstream>
#include <iostream>

#include "log2word/solver/feedback.hpp"

int main()
{
      log2word::core c("data/all.txt","data/answers.txt");
      c.pre_calculate(true);

      log2word::solver::feedback f(c.all_words[0],c.answers[0]);
      f.debug_output();
      c.all_to_answer_feedbackLUT[0][0].debug_output();
      std::cout << c.all_words[0] << std::endl;
      std::cout << c.answers[0] << std::endl;
}
