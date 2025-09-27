//
// Created by moosm on 9/26/2025.
//


#ifndef LOGGER_HPP
#define LOGGER_HPP


#include <vector>
#include <string>


namespace log2word::common::io
{
    /// @param list Word list.
    /// @param top [optional] the first 'top' elements to log.
    /// @param stream [optional] Stream.
    static inline void log_list
    (const std::vector<std::string>& list, const int top = 4, std::ostream& stream = std::cout)
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
