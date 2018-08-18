#ifndef CODE_GENERATOR_CPP_FILE_GENERATOR_H
#define CODE_GENERATOR_CPP_FILE_GENERATOR_H

#include <string>
#include <utility>
#include <fstream>
#include "ast.h"
#include "replacer.h"

namespace code_generator
{
    class CppFileGenerator
    {
    public:
        CppFileGenerator(std::string output_file_name, std::string includes_file_name,
                         std::string msg_types_header_file_name, std::string declarations_file_name,
                         std::string key_handler_declaration_file_name,
                         std::string setup_func_file_name, std::string can_send_task_file_name,
                         std::string timer_task_code_file_name, std::string pins_reader_task_code_file_name)
                : output_file_name(std::move(output_file_name)), includes_file_name(std::move(includes_file_name)),
                  msg_types_header_file_name(std::move(msg_types_header_file_name)), declarations_file_name(std::move(declarations_file_name)),
                  key_handler_declaration_file_name(std::move(key_handler_declaration_file_name)),
                  setup_func_file_name(std::move(setup_func_file_name)), can_send_task_file_name(std::move(can_send_task_file_name)),
                  timer_task_code_file_name(std::move(timer_task_code_file_name)), pins_reader_task_code_file_name(std::move(pins_reader_task_code_file_name))
        {}

        void generate(const std::vector<std::string>& functions,
                      const std::vector<code_generator::util::TimerTask>& timer_tasks,
                      const std::vector<code_generator::util::KeyTask>& key_tasks,
                      const std::string& global_variables_declaration)
        {
            std::ofstream out_file{output_file_name};
            code_generator::Replacer includes_replacer{includes_file_name};
            includes_replacer.add_tag("MsgTypesHeaderFileName", msg_types_header_file_name);
            includes_replacer.replace_tags();
            out_file << includes_replacer;
            std::stringstream key_handlers_prototype_list;
            std::stringstream key_handlers_declaration_list;
            auto first_time_key_task_loop = true;
            for(const auto& key_task : key_tasks)
            {
                if(first_time_key_task_loop)
                {
                    first_time_key_task_loop = false;
                } else
                {
                    key_handlers_prototype_list << "\n";
                    key_handlers_declaration_list << ",\n";
                }
                Replacer key_handler_declaration_replacer{key_handler_declaration_file_name};
                key_handler_declaration_replacer.add_tag("DigitalInputPin", std::to_string(key_task.pin))
                        .add_tag("KeyHandler", key_task.task_name);
                key_handler_declaration_replacer.replace_tags();
                key_handlers_prototype_list << "void " << key_task.task_name << "(" << key_task.task_parameter << ");";
                key_handlers_declaration_list << "\t" << key_handler_declaration_replacer;
            }
            std::stringstream declare_task_list;
            std::stringstream send_msg_task_list;
            auto first_time_timer_task_loop = true;
            for(const auto& timer_task : timer_tasks)
            {
                if(first_time_timer_task_loop)
                {
                    first_time_timer_task_loop = false;
                } else
                {
                    declare_task_list << "\n";
                    send_msg_task_list << ",\n";
                }
                declare_task_list << "DeclareTask("
                                  << timer_task.task_name
                                  << ");";
                send_msg_task_list << "\t" << timer_task.task_name;
            }
            code_generator::Replacer declarations_replacer{declarations_file_name};
            declarations_replacer.add_tag("DeclareTaskList", declare_task_list.str())
                                 .add_tag("KeyHandlersPrototypeList", key_handlers_prototype_list.str())
                                 .add_tag("SendMsgTaskList", send_msg_task_list.str())
                                 .add_tag("KeyHandlersDeclarationList", key_handlers_declaration_list.str());
            declarations_replacer.replace_tags();
            out_file << declarations_replacer;
            out_file << global_variables_declaration << "\n\n";
            out_file << std::ifstream(setup_func_file_name).rdbuf()
                     << std::ifstream(can_send_task_file_name).rdbuf();
            for(const auto& timer_task : timer_tasks)
            {
                code_generator::Replacer timer_task_code_replacer{timer_task_code_file_name};
                timer_task_code_replacer.add_tag("TaskName", timer_task.task_name)
                        .add_tag("TaskInnerCode", timer_task.inner_code);
                timer_task_code_replacer.replace_tags();
                out_file << timer_task_code_replacer;
            }
            for(const auto& key_task : key_tasks)
            {
                std::stringstream key_task_code;
                key_task_code << "void " << key_task.task_name
                              << "(" << key_task.task_parameter << ")\n{"
                              << key_task.task_inner_code << "}\n\n";
                out_file << key_task_code.str();
            }
            out_file << util::read_whole_file_as_string(pins_reader_task_code_file_name);
            for(const auto& function : functions)
            {
                out_file << function << "\n\n";
            }
        }
    private:
        std::string output_file_name;
        std::string includes_file_name;
        std::string msg_types_header_file_name;
        std::string declarations_file_name;
        std::string key_handler_declaration_file_name;
        std::string setup_func_file_name;
        std::string can_send_task_file_name;
        std::string timer_task_code_file_name;
        std::string pins_reader_task_code_file_name;
    };
}

#endif //CODE_GENERATOR_CPP_FILE_GENERATOR_H
