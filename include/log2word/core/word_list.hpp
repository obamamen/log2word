// -----------------------------------------------------------------------------
//  
//  log2word - world_list.hpp
//
// -----------------------------------------------------------------------------


#ifndef LOG2WORD_WORD_LIST
#define LOG2WORD_WORD_LIST
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "log2word/common/io/file_loader.hpp"

namespace log2word
{
    class word_list
    {
    public:
        using word_index_t = uint16_t;
        using answer_index_t = uint16_t;
    public:
        word_list() = default;
        ~word_list() = default;
        word_list(const word_list&) = default;
        word_list& operator=(const word_list&) = default;
        word_list(word_list&&) = default;
        word_list& operator=(word_list&&) = default;

        explicit word_list(const std::string& all_words_path, const std::string& answers_path = "")
        {
            if (!common::io::load_word_file_to_list(_all_words_list, all_words_path))
            {
                throw std::runtime_error("failed to load all_words_list");
            }

            if (!(common::io::load_word_file_to_list(_answers_list,
                                                               (answers_path.empty()) ? all_words_path : answers_path)))
            {
                throw std::runtime_error("failed to load answer_list");
            }

            _answer_to_word.resize(_answers_list.size());

            std::unordered_map<std::string, word_index_t> answer_map;
            for (auto w = 0ull; w < _all_words_list.size(); ++w) answer_map[ _all_words_list[w] ]   = static_cast<answer_index_t>(w);
            for (auto a = 0ull; a < _answer_to_word.size(); ++a) _answer_to_word[a]                 = answer_map[ _answers_list[a] ];
        }

        word_index_t answer_index_to_word_index(const answer_index_t answer) const noexcept
        {
            assert(answer < _answer_to_word.size());
            return _answer_to_word[answer];
        }

        const std::string& get_word(const word_index_t word) const noexcept
        {
            assert(word < _all_words_list.size());
            return _all_words_list[word];
        }

        const std::string& get_answer(const answer_index_t answer) const noexcept
        {
            return _all_words_list[answer_index_to_word_index(answer)];
        }

        const std::vector<std::string>& get_all_words() const noexcept
        {
            return _all_words_list;
        }

        const std::vector<std::string>& get_answers() const noexcept
        {
            return _answers_list;
        }

    private:
        std::vector<std::string> _all_words_list{};
        std::vector<std::string> _answers_list{};
        std::vector<word_index_t> _answer_to_word{};
    };
}

#endif // LOG2WORD_WORD_LIST