//
// Created by moosm on 9/26/2025.
//


#ifndef FILE_LOADER_HPP
#define FILE_LOADER_HPP


#include <vector>
#include <string>


namespace log2word::common::io
{
    /// @param list List to load the words into (should be empty but could work without, just appends)
    /// @param path File path to load
    /// @return Returns load success
    static bool load_word_file_to_list(std::vector<std::string>& list, const std::string& path)
    {
        std::fstream file(path);
        if (!file.is_open()) return false;

        std::string line;
        while (true)
        {
            std::getline(file, line);

            if (file.fail()) return false;
            if (file.eof()) break;

            if (line.empty()) continue;
            if (line[0] == '#') continue;

            list.push_back(line);
        }
        return true;
    }
}
#endif //FILE_LOADER_HPP
