#ifndef CODE_GENERATOR_AST_H
#define CODE_GENERATOR_AST_H

#include <string>
#include <vector>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

namespace code_generator
{
    namespace ast
    {
        struct TimerHandler
        {
            std::string name;
            unsigned int milliseconds;
        };

        struct KeyHandler
        {
            std::string key;
        };

        struct MessageHandlerPgnAll
        {
            bool declared{false};
        };

        struct AnyHandler
        {
            boost::spirit::x3::variant<TimerHandler, KeyHandler, MessageHandlerPgnAll, std::string> handler;
        };

        struct SignalValue
        {
            std::string description;
            long unsigned int value;
        };

        struct Signal
        {
            std::string name;
            unsigned int length;
            unsigned int byte_index;
            unsigned int bit_no;
            char type;
            long long int max_val;
            long long int min_val;
            unsigned int byte_order;
            double offset;
            double scale_factor;
            std::string unit;
            std::vector<SignalValue> signal_values;
        };

        struct Message
        {
            std::string name;
            unsigned int pgn;
            unsigned int length;
            unsigned int n_signals;
            std::vector<Signal> signals;
        };

        struct Dbf
        {
            std::vector<char> unused_info;
            std::vector<Message> messages;
        };

        struct HandlerFunction
        {
            std::string name;
            std::vector<std::string> parameters;
            std::string inner_code;
        };

        struct PinProperties
        {
            bool digital;
            unsigned int pin_number;
            bool active_state_high;
        };

        using boost::fusion::operator<<;
    }
}

BOOST_FUSION_ADAPT_STRUCT(code_generator::ast::TimerHandler,
        name, milliseconds
)

BOOST_FUSION_ADAPT_STRUCT(code_generator::ast::KeyHandler,
        key
)

BOOST_FUSION_ADAPT_STRUCT(code_generator::ast::MessageHandlerPgnAll,
        declared
)

BOOST_FUSION_ADAPT_STRUCT(code_generator::ast::AnyHandler,
        handler
)

BOOST_FUSION_ADAPT_STRUCT(code_generator::ast::SignalValue,
        description, value
)

BOOST_FUSION_ADAPT_STRUCT(code_generator::ast::Signal,
        name, length, byte_index,
        bit_no, type, max_val, min_val,
        byte_order, offset, scale_factor,
        unit, signal_values
)

BOOST_FUSION_ADAPT_STRUCT(code_generator::ast::Message,
        name, pgn, length, n_signals, signals
)

BOOST_FUSION_ADAPT_STRUCT(code_generator::ast::Dbf,
        unused_info, messages
)

BOOST_FUSION_ADAPT_STRUCT(code_generator::ast::HandlerFunction,
        name, parameters, inner_code
)

BOOST_FUSION_ADAPT_STRUCT(code_generator::ast::PinProperties,
        digital, pin_number, active_state_high
)

#endif //CODE_GENERATOR_AST_H
