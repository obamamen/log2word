//
// Created by moosm on 9/26/2025.
//

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "../log2word__project.hpp"

namespace LOG2WORD_NAMESPACE::logger
{
    static inline void log_list
    (std::ostream& stream, const std::vector<std::string>& list, const int top = 1)
    {
        if (list.size() < top) return;
        int i = 1;
        for (; i < top; i++)
        {
            stream << i << ": " << list[i-1] << '\n';
        }
        stream << i << ": " << "..." << std::endl;
    }
}

#endif //LOGGER_HPP
