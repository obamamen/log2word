//
// Created by moosm on 9/25/2025.
//

#include "log2word/log2word.hpp"

#include <fstream>
#include <iostream>

int main()
{
      const log2word::core c("data/all.txt","data/answers.txt");
      c.debug_output_lists();
}
