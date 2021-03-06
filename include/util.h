#ifndef CODE_GENERATOR_UTIL_H
#define CODE_GENERATOR_UTIL_H

#include <iostream>
#include <fstream>
#include <boost/spirit/home/support/iterators/istream_iterator.hpp>
#include "parser.h"
#include <boost/fusion/include/std_pair.hpp>
#include <QFile>
#include <QTextStream>
#include <QValidator>
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
                              std::vector<ast::MessageHandlerPgnName>& pgn_name_msgs,
                              ast::MessageHandlerPgnAll &m_pgn_all,
                              ast::OnDllLoadHandler& dll_load)
                : timers(timers), keys(keys), pgn_name_msgs(pgn_name_msgs)
                , m_pgn_all(m_pgn_all), dll_load(dll_load)
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

            void operator()(code_generator::ast::MessageHandlerPgnName m)
            {
                pgn_name_msgs.push_back(m);
            }

            void operator()(code_generator::ast::OnDllLoadHandler d)
            {
                dll_load = d;
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
            std::vector<code_generator::ast::MessageHandlerPgnName>& pgn_name_msgs;
            code_generator::ast::MessageHandlerPgnAll& m_pgn_all;
            code_generator::ast::OnDllLoadHandler& dll_load;
        };

        struct TimerTask
        {
            std::string timer_name;
            std::string task_name;
            unsigned int milliseconds;
            std::string task_inner_code;
        };

        struct KeyTask
        {
            std::string key;
            unsigned int pin;
            bool digital;
            bool active_state_high;
            std::string task_name;
            std::string task_parameter;
            std::string task_inner_code;
        };

        struct PgnNameTask
        {
            std::string type_name;
            std::string task_name;
            std::string task_inner_code;
        };

        struct PgnAllTask
        {
            std::string task_name;
            std::string task_parameter;
            std::string task_inner_code;
        };

        struct DllLoadTask
        {
            std::string task_name;
            std::string task_inner_code;
        };

        struct SystemTasksInfo
        {
            SystemTasksInfo() = default;
            SystemTasksInfo(const SystemTasksInfo&) = default;
            SystemTasksInfo(SystemTasksInfo&&) = default;
            SystemTasksInfo& operator=(const SystemTasksInfo&) = default;
            SystemTasksInfo& operator=(SystemTasksInfo&&) = default;
            SystemTasksInfo(unsigned int _total_timer_tasks, bool _pins_reader_task_used,
                            bool _can_send_task_used, bool _message_handler_pgn_all_used,
                            bool _message_handler_pgn_name_used, bool _dll_load_handler_used, bool _serial_used)
                : total_timer_tasks{_total_timer_tasks}, pins_reader_task_used{_pins_reader_task_used}
                , can_send_task_used{_can_send_task_used}
                , can_recv_task_used{_message_handler_pgn_all_used || _message_handler_pgn_name_used}
                , timer_task_used{_total_timer_tasks != 0}, message_handler_pgn_all_used{_message_handler_pgn_all_used}
                , message_handler_pgn_name_used{_message_handler_pgn_name_used}
                , dll_load_handler_used{_dll_load_handler_used}, serial_used{_serial_used}
            {}

            unsigned int number_of_tasks() const noexcept
            {
                return total_timer_tasks + (pins_reader_task_used ? 1 : 0)
                        + (can_send_task_used ? 1 : 0) + (can_recv_task_used ? 1 : 0);
            }

            unsigned int total_timer_tasks{0};
            bool pins_reader_task_used{false};
            bool can_send_task_used{false};
            bool can_recv_task_used{false};
            bool timer_task_used{false};
            bool message_handler_pgn_all_used{false};
            bool message_handler_pgn_name_used{false};
            bool dll_load_handler_used{false};
            bool serial_used{false};
        };

        inline std::tuple<
                std::vector<code_generator::ast::TimerHandler>,
                std::vector<code_generator::ast::KeyHandler>,
                std::vector<code_generator::ast::MessageHandlerPgnName>,
                code_generator::ast::MessageHandlerPgnAll,
                code_generator::ast::OnDllLoadHandler
        > read_def(const std::string& file_name)
        {
            using boost::spirit::x3::ascii::space;
            using code_generator::parser::any_handler;
            std::ifstream in_file(file_name);
            std::string line;
            std::vector<code_generator::ast::TimerHandler> timers;
            std::vector<code_generator::ast::KeyHandler> keys;
            std::vector<code_generator::ast::MessageHandlerPgnName> pgn_name_msgs;
            code_generator::ast::MessageHandlerPgnAll m_pgn_all;
            code_generator::ast::OnDllLoadHandler on_dll_load;
            HandlersExtractor handlers_extractor{timers, keys, pgn_name_msgs, m_pgn_all, on_dll_load};
            while(std::getline(in_file,line))
            {
                code_generator::ast::AnyHandler handler;
                phrase_parse(std::begin(line), std::end(line), any_handler, space, handler);
                handlers_extractor(handler);
            }
            return std::make_tuple(std::move(timers), std::move(keys), std::move(pgn_name_msgs), std::move(m_pgn_all), std::move(on_dll_load));
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
                std::vector<code_generator::util::PgnNameTask>,
                code_generator::util::PgnAllTask,
                code_generator::util::DllLoadTask,
                std::vector<std::string>,
                QString
        > get_functions(const std::string &file_name,
                        const std::vector<code_generator::ast::TimerHandler>& timer_handlers,
                        const std::vector<code_generator::ast::KeyHandler>& key_handlers,
                        const std::vector<code_generator::ast::MessageHandlerPgnName>& pgn_name_msgs,
                        const std::map<std::string, ast::PinProperties>& key_mapping,
                        const code_generator::ast::MessageHandlerPgnAll& m_pgn_all,
                        const code_generator::ast::OnDllLoadHandler& on_dll_load,
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
            std::vector<code_generator::util::PgnNameTask> pgn_name_tasks;
            code_generator::util::PgnAllTask msg_handler_pgn_all;
            code_generator::util::DllLoadTask on_dll_load_task;
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
                    auto pgn_name_handler = std::find_if(pgn_name_msgs.begin(), pgn_name_msgs.end(),
                                        [&function](const auto& pgn_name_handler)
                                        {
                                            std::stringstream ss;
                                            ss << "OnPGNName_" << pgn_name_handler.type_name;
                                            return ss.str() == function.name;
                                        });
                    bool is_msg_handler_pgn_all = m_pgn_all.declared && function.name == "OnPGN_All";
                    bool is_on_dll_load_handler = on_dll_load.declared && function.name == "OnDLL_Load";
                    if(timer_handlers.end() != timer_handler)
                    {
                        timer_tasks.push_back(TimerTask{timer_handler->name, function.name, timer_handler->milliseconds, function.inner_code});
                    } else if(key_handlers.end() != key_handler)
                    {
                        if(key_mapping.find(function.name) != key_mapping.end())
                        {
                            auto value = key_mapping.at(function.name);
                            key_tasks.push_back(KeyTask{key_handler->key, value.pin_number, value.digital, value.active_state_high, function.name, function.parameters[0], function.inner_code});
                        } else
                        {
                            error_ts << "Erro: A função " << QString::fromStdString(function.name) << " não existe no mapeamento de teclas para pinos\n";
                            error = true;
                        }
                    } else if(pgn_name_msgs.end() != pgn_name_handler)
                    {
                        pgn_name_tasks.push_back(PgnNameTask{pgn_name_handler->type_name, function.name, function.inner_code});
                    } else if(is_msg_handler_pgn_all)
                    {
                        msg_handler_pgn_all.task_name = function.name;
                        msg_handler_pgn_all.task_parameter = function.parameters[0];
                        msg_handler_pgn_all.task_inner_code = function.inner_code;
                    } else if(is_on_dll_load_handler)
                    {
                        on_dll_load_task.task_name = function.name;
                        on_dll_load_task.task_inner_code = function.inner_code;
                    } else {
                        functions.push_back(text);
                    }
                } else
                {
                    functions.push_back(text);
                }
            }
            return std::make_tuple(timer_tasks,key_tasks,pgn_name_tasks,msg_handler_pgn_all,on_dll_load_task,functions,error_ts.readAll());
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

        inline auto get_key_mapping(const std::string& key_mapping_str)
        {
            using parser::key_pin_map_rule;
            using boost::spirit::x3::space;
            std::map<std::string, ast::PinProperties> key_pin_map;
            phrase_parse(key_mapping_str.begin(), key_mapping_str.end(), key_pin_map_rule, space, key_pin_map);
            return key_pin_map;
        }

        class HexByteValidator : public QValidator
        {
            QValidator::State validate(QString& input, int&) const
            {
                QRegExp rxAcceptable("^([0-9a-fA-F]){1,2}$");
                QRegExp rxIntermediate("^$");
                if(rxAcceptable.exactMatch(input)) {
                    return QValidator::Acceptable;
                } else if (rxIntermediate.exactMatch(input)) {
                    return QValidator::Intermediate;
                }
                return QValidator::Invalid;
            }
        };
    };
}

#endif //CODE_GENERATOR_UTIL_H
