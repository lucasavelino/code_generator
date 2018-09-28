#ifndef CODE_GENERATOR_UTIL_H
#define CODE_GENERATOR_UTIL_H

#include <iostream>
#include <fstream>
#include <boost/spirit/home/support/iterators/istream_iterator.hpp>
#include "parser.h"
#include <boost/fusion/include/std_pair.hpp>
#include <QFile>
#include <QTextStream>
#include <QDebug>

namespace code_generator
{
    namespace util
    {
        struct HandlersExtractor
        {
            using result_type = void;

            HandlersExtractor(std::vector<ast::TimerHandler> &timers,
                              std::vector<ast::KeyHandler> &keys,
                              ast::MessageHandlerPgnAll &m_pgn_all)
                : timers(timers), keys(keys), m_pgn_all(m_pgn_all)
            {}

            void operator()(const code_generator::ast::TimerHandler& t)
            {
                timers.push_back(t);
            }

            void operator()(const code_generator::ast::KeyHandler& k)
            {
                keys.push_back(k);
            }

            void operator()(code_generator::ast::MessageHandlerPgnAll m)
            {
                m_pgn_all = m;
            }

            void operator()(const std::string& unused_info)
            {
                (void)unused_info;
            }

            void operator()(const code_generator::ast::AnyHandler& any_handler)
            {
                any_handler.handler.apply_visitor(*this);
            }

            std::vector<code_generator::ast::TimerHandler>& timers;
            std::vector<code_generator::ast::KeyHandler>& keys;
            code_generator::ast::MessageHandlerPgnAll& m_pgn_all;
        };

        struct TimerTask
        {
            std::string timer_name;
            std::string task_name;
            unsigned int milliseconds;
            std::string inner_code;
        };

        struct KeyTask
        {
            std::string key;
            unsigned int pin;
            std::string task_name;
            std::string task_parameter;
            std::string task_inner_code;
        };

        struct PgnAllTask
        {
            std::string task_name;
            std::string task_parameter;
            std::string task_inner_code;
        };

        inline std::tuple<
                std::vector<code_generator::ast::TimerHandler>,
                std::vector<code_generator::ast::KeyHandler>,
                code_generator::ast::MessageHandlerPgnAll
        > read_def(const std::string& file_name)
        {
            using boost::spirit::x3::ascii::space;
            using code_generator::parser::any_handler;
            std::ifstream in_file(file_name);
            std::string line;
            std::vector<code_generator::ast::TimerHandler> timers;
            std::vector<code_generator::ast::KeyHandler> keys;
            code_generator::ast::MessageHandlerPgnAll m_pgn_all;
            HandlersExtractor handlers_extractor{timers, keys, m_pgn_all};
            while(std::getline(in_file,line))
            {
                code_generator::ast::AnyHandler handler;
                phrase_parse(std::begin(line), std::end(line), any_handler, space, handler);
                handlers_extractor(handler);
            }
            return std::make_tuple(std::move(timers), std::move(keys), std::move(m_pgn_all));
        }

        inline std::vector<code_generator::ast::Message> read_dbf(const std::string& file_name)
        {
            using boost::spirit::x3::ascii::space;
            using code_generator::parser::dbf;
            code_generator::ast::Dbf dbf_msgs;
            std::ifstream in_file(file_name);
            boost::spirit::istream_iterator iter(in_file), eof;
            phrase_parse(iter, eof, dbf, space, dbf_msgs);
            return std::move(dbf_msgs.messages);
        }

        inline std::string read_whole_file_as_string(const std::string& file_name)
        {
            std::ifstream t(file_name);
            std::string str;

            t.seekg(0, std::ios::end);
            str.reserve(static_cast<unsigned int>(t.tellg()));
            t.seekg(0, std::ios::beg);

            str.assign((std::istreambuf_iterator<char>(t)),
                       std::istreambuf_iterator<char>());
            return str;
        }

        inline QString read_resource(const QString& file_name)
        {
            QFile file(file_name);
            if(!file.open(QFile::ReadOnly | QFile::Text))
            {
                qDebug() << "Could not open file " << file_name;
                return "";
            }
            return QTextStream(&file).readAll();
        }

        inline std::vector<std::string> get_text_inside_delimited_block(const std::string& file_str, std::string _delimiter)
        {
            const std::string start = "/* Start " + _delimiter;
            const std::string end = "/* End " + _delimiter;
            const std::string end_comment = "*/\n";
            std::vector<std::string> texts;
            std::string::size_type iter = 0;
            while(iter != std::string::npos)
            {
                auto first = file_str.find(start,iter);
                if(first == std::string::npos)
                {
                    break;
                }
                first = file_str.find(end_comment, first);
                if(first == std::string::npos)
                {
                    break;
                }
                first += end_comment.length();
                auto last = file_str.find(end, first);
                if(last == std::string::npos)
                {
                    break;
                }
                texts.emplace_back(file_str, first, last-first);
                last = file_str.find(end_comment, last);
                if(last == std::string::npos)
                {
                    break;
                }
                iter = last + end_comment.length();
            }
            return texts;
        }

        inline std::tuple<
                std::vector<code_generator::util::TimerTask>,
                std::vector<code_generator::util::KeyTask>,
                code_generator::util::PgnAllTask,
                std::vector<std::string>,
                QString
        > get_functions(const std::string &file_name,
                        const std::vector<code_generator::ast::TimerHandler>& timer_handlers,
                        const std::vector<code_generator::ast::KeyHandler>& key_handlers,
                        const std::map<std::string, unsigned int>& key_mapping,
                        code_generator::ast::MessageHandlerPgnAll& m_pgn_all,
                        bool& error)
        {
            error = false;
            QString error_str;
            QTextStream error_ts(&error_str);
            using boost::spirit::x3::ascii::space;
            using code_generator::parser::handler_function;
            std::vector<std::string> functions;
            std::vector<code_generator::util::TimerTask> timer_tasks;
            std::vector<code_generator::util::KeyTask> key_tasks;
            code_generator::util::PgnAllTask msg_handler_pgn_all;
            const auto cpp_file_str{read_whole_file_as_string(file_name)};
            auto texts = get_text_inside_delimited_block(cpp_file_str, "J1939 generated function");
            for(const auto& text : texts)
            {
                code_generator::ast::HandlerFunction function;
                if(phrase_parse(text.begin(), text.end(), handler_function, space, function))
                {
                    auto timer_handler = std::find_if(timer_handlers.begin(), timer_handlers.end(),
                                 [&function](const auto& timer_handler)
                                 {
                                     std::stringstream ss;
                                     ss << "OnTimer_"
                                        << timer_handler.name << "_"
                                        << timer_handler.milliseconds;
                                     return ss.str() == function.name;
                                 });
                    auto key_handler = std::find_if(key_handlers.begin(), key_handlers.end(),
                                 [&function](const auto& key_handler)
                                 {
                                     std::stringstream ss;
                                     ss << "OnKey_" << key_handler.key;
                                     return ss.str() == function.name;
                                 });
                    bool is_msg_handler_pgn_all = m_pgn_all.declared && function.name == "OnPGN_All";
                    if(timer_handlers.end() != timer_handler)
                    {
                        timer_tasks.push_back(TimerTask{timer_handler->name, function.name, timer_handler->milliseconds, function.inner_code});
                    } else if(key_handlers.end() != key_handler)
                    {
                        if(key_mapping.find(function.name) != key_mapping.end())
                        {
                            key_tasks.push_back(KeyTask{key_handler->key, key_mapping.at(function.name), function.name, function.parameters[0], function.inner_code});
                        } else
                        {
                            error_ts << "Erro: A função " << QString::fromStdString(function.name) << " não existe no mapeamento de teclas para pinos\n";
                            error = true;
                        }
                    } else if(is_msg_handler_pgn_all)
                    {
                        msg_handler_pgn_all.task_name = function.name;
                        msg_handler_pgn_all.task_parameter = function.parameters[0];
                        msg_handler_pgn_all.task_inner_code = function.inner_code;
                    }else
                    {
                        functions.push_back(text);
                    }
                }
            }
            return std::make_tuple(timer_tasks,key_tasks,msg_handler_pgn_all,functions,error_ts.readAll());
        }

        inline std::string get_global_variables_declaration(const std::string& file_name)
        {
            const auto cpp_file_str{read_whole_file_as_string(file_name)};
            auto texts = get_text_inside_delimited_block(cpp_file_str, "J1939 global variable");
            return std::move(texts[0]);
        }

        inline std::string get_appropriate_signal_type(const code_generator::ast::Signal& sig)
        {
            if(sig.type == 'B')
            {
                return "bool";
            }

            if(sig.length <= 8)
            {
                if(sig.type == 'U')
                {
                    return "uint8_t";
                } else
                {
                    return "int8_t";
                }
            } else if(sig.length <= 16)
            {
                if(sig.type == 'U')
                {
                    return "uint16_t";
                } else
                {
                    return "int16_t";
                }
            } else if(sig.length <= 32)
            {
                if(sig.type == 'U')
                {
                    return "uint32_t";
                } else
                {
                    return "int32_t";
                }
            } else
            {
                if(sig.type == 'U')
                {
                    return "uint64_t";
                } else
                {
                    return "int64_t";
                }
            }
        }

        inline std::map<std::string, unsigned int> get_key_mapping(const std::string& key_mapping_str)
        {
            namespace x3 = boost::spirit::x3;
            namespace ascii = boost::spirit::x3::ascii;
            using boost::spirit::x3::ascii::space;
            using ascii::char_;
            using x3::int_;
            auto key_pin_item_rule = x3::rule<class key_pin_item_rule, std::pair<std::string, unsigned int>>() =
                    *(char_ - ':') >> ':' >> int_;
            auto key_pin_map_rule = x3::rule<class key_pin_map_rule, std::map<std::string, unsigned int>>() =
                    '{'
                    >> (key_pin_item_rule % ',')
                    >> '}';
            std::map<std::string, unsigned int> key_pin_map;
            phrase_parse(key_mapping_str.begin(), key_mapping_str.end(), key_pin_map_rule, space, key_pin_map);
            return key_pin_map;
        }
    };
}

#endif //CODE_GENERATOR_UTIL_H
