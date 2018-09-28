#ifndef CODE_GENERATOR_PARSER_H
#define CODE_GENERATOR_PARSER_H

#include "ast.h"

namespace code_generator
{
    namespace parser
    {
        namespace x3 = boost::spirit::x3;
        namespace ascii = boost::spirit::x3::ascii;
        using x3::int_;
        using x3::lit;
        using x3::double_;
        using x3::lexeme;
        using ascii::char_;
        using x3::long_long;
        using x3::matches;
        using x3::_attr;
        using x3::_val;
        using x3::_pass;

        x3::rule<class timer_handler, ast::TimerHandler> const timer_handler = "timer_handler";
        auto const timer_handler_def =
                lexeme[
                        lit("OnTimer")
                                >> '_'
                                >> +(char_ - '_')
                                >> '_'
                                >> int_
                ];
        BOOST_SPIRIT_DEFINE(timer_handler);

        x3::rule<class key_handler, ast::KeyHandler> const key_handler = "key_handler";
        auto const key_handler_def =
                lexeme[
                        lit("OnKey")
                        >> '_'
                ]
                >> +(char_);
        BOOST_SPIRIT_DEFINE(key_handler);

        auto declared_if_matches =
            [](auto& ctx)
            {
                _val(ctx).declared = true;
            };

        x3::rule<class msg_handler_pgn_all, ast::MessageHandlerPgnAll> const msg_handler_pgn_all = "msg_handler_pgn_all";
        auto const msg_handler_pgn_all_def =
                lexeme[
                    lit("OnPGN_All")[declared_if_matches]
                ];
        BOOST_SPIRIT_DEFINE(msg_handler_pgn_all);

        x3::rule<class any_handler, ast::AnyHandler> const any_handler = "any_handler";
        auto const any_handler_def =
                (timer_handler | key_handler | msg_handler_pgn_all | *char_);
        BOOST_SPIRIT_DEFINE(any_handler);

        x3::rule<class signal_value, ast::SignalValue> const signal_value = "signal_value";
        auto const signal_value_def =
                lit("[VALUE_DESCRIPTION]")
                >> +(char_ - ',')
                >> ','
                >> int_;
        BOOST_SPIRIT_DEFINE(signal_value);

        x3::rule<class signal, ast::Signal> const signal = "signal";
        auto const signal_def =
                lit("[START_SIGNALS]")
                >> +(char_ - ',')
                >> ','
                >> int_
                >> ','
                >> int_
                >> ','
                >> int_
                >> ','
                >> char_
                >> ','
                >> long_long
                >> ','
                >> long_long
                >> ','
                >> int_
                >> ','
                >> double_
                >> ','
                >> double_
                >> ','
                >> *(char_ - ',')
                >> ','
                >> *signal_value;
        BOOST_SPIRIT_DEFINE(signal);

        x3::rule<class message, ast::Message> const message = "message";
        auto const message_def =
                lit("[START_MSG]")
                >> +(char_ - ',')
                >> ','
                >> int_
                >> ','
                >> int_
                >> ','
                >> int_
                >> lit(",1,X")
                >> *signal
                >> lit("[END_MSG]");
        BOOST_SPIRIT_DEFINE(message);

        x3::rule<class dbf, ast::Dbf> const dbf = "dbf";
        auto const dbf_def =
                *(char_ - "[START_MSG]")
                >> *message;
        BOOST_SPIRIT_DEFINE(dbf);

        x3::rule<class handler_function, ast::HandlerFunction> const handler_function = "handler_function";
        auto const handler_function_def =
                lit("void")
                >> +(char_ - '(')
                >> '('
                >> lexeme[(*(char_ - (char_(',') | char_(')'))))] % ','
                >> ')'
                >> '{'
                >> lexeme[+(char_ - '}')]
                >> '}';
        BOOST_SPIRIT_DEFINE(handler_function);
    }
}

#endif //CODE_GENERATOR_PARSER_H
