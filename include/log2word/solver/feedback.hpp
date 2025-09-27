// -----------------------------------------------------------------------------
//  
//  log2word - feedback.hpp
//
// -----------------------------------------------------------------------------


#ifndef LOG2WORD_FEEDBACK
#define LOG2WORD_FEEDBACK


#include <array>
#include <cstdint>
#include <iostream>
#include <string_view>


namespace log2word::solver
{
    struct feedback
    {
        enum class feedback_type { green = 0b10, yellow = 0b01, grey = 0b00 };

        static char feedback_type_to_char(const feedback_type type)
        {
            switch (type)
            {
                case feedback_type::green: return 'g';
                case feedback_type::yellow: return 'y';
                default: return 'x';
            }
        }

        static feedback_type feedback_type_from_char(const char c)
        {
            switch (c)
            {
                case 'g': return feedback_type::green;
                case 'y': return feedback_type::yellow;
                case 'x': return feedback_type::grey;
                default: throw std::invalid_argument("invalid character in feedback_type_from_char");
            }
        }

        inline static uint16_t bit_mask = 0b11;

    private:

        uint16_t feedback_bits{};

    public:

        feedback() = default;

        feedback(const std::string_view guess, const std::string_view answer)
        {
            if (guess.length() != answer.length()) return;
            std::array<bool,5> used{};

            for (int i = 0; i < 5; i++)
            {
                if (guess[i] == answer[i])
                {
                    used[i] = true;
                    this->set(i,feedback_type::green);
                }
            }

            for (int i = 0; i < 5; i++)
            {
                if (this->get(i) != feedback_type::grey) continue;
                for (int j = 0; j < 5; j++)
                {
                    if (used[j] == true) continue;
                    if (guess[i] == answer[j])
                    {
                        this->set(i,feedback_type::yellow);
                        used[j] = true;
                        break;
                    }
                }
            }
        }

        [[nodiscard]] feedback_type get(const int index) const
        {
            const uint16_t mask = bit_mask << (index * 2);
            uint16_t bits = (feedback_bits & mask) >> (index * 2);
            return static_cast<feedback_type>(bits);
        }

        void set(const int index, feedback_type value)
        {
            const uint16_t clear_mask = ~(bit_mask << (index * 2));
            feedback_bits &= clear_mask;

            const uint16_t set_bits = static_cast<uint16_t>(value) << (index * 2);
            feedback_bits |= set_bits;
        }

        void debug_output(std::ostream& stream = std::cout) const
        {
            for (int i = 0; i < 5; i++)
            {
                stream << feedback_type_to_char(get(i));
            }
            stream << std::endl;
        }

        ~feedback() = default;
        feedback(const feedback&) = default;
        feedback(feedback&&) = default;
        feedback& operator=(const feedback&) = default;
        feedback& operator=(feedback&&) = default;
    };
}

#endif // LOG2WORD_FEEDBACK