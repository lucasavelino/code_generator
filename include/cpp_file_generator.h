#ifndef CODE_GENERATOR_CPP_FILE_GENERATOR_H
#define CODE_GENERATOR_CPP_FILE_GENERATOR_H

#include <utility>
#include "ast.h"
#include "replacer.h"
#include <QString>

namespace code_generator
{
    class CppFileGenerator
    {
    public:
        CppFileGenerator(QString output_file_name, QString includes_file_name,
                         QString msg_types_header_file_name, QString declarations_file_name,
                         QString key_handler_declaration_file_name,
                         QString setup_func_file_name, QString can_send_task_file_name,
                         QString timer_task_code_file_name, QString pins_reader_task_code_file_name)
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
            QFile out_file{output_file_name};
            if(!out_file.open(QFile::WriteOnly | QFile::Text))
            {
                qDebug() << "Could not open file " << output_file_name;
                return;
            }
            QTextStream out_file_stream(&out_file);
            code_generator::Replacer includes_replacer{includes_file_name};
            includes_replacer.add_tag("MsgTypesHeaderFileName", msg_types_header_file_name);
            includes_replacer.replace_tags();
            out_file_stream << includes_replacer;
            QString key_handlers_prototype_list_str;
            QString key_handlers_declaration_list_str;
            QTextStream key_handlers_prototype_list(&key_handlers_prototype_list_str);
            QTextStream key_handlers_declaration_list(&key_handlers_declaration_list_str);
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
                key_handler_declaration_replacer.add_tag("DigitalInputPin", QString::number(key_task.pin))
                        .add_tag("KeyHandler", key_task.task_name.c_str());
                key_handler_declaration_replacer.replace_tags();
                key_handlers_prototype_list << "void " << key_task.task_name.c_str() << "(" << key_task.task_parameter.c_str() << ");";
                key_handlers_declaration_list << "\t" << key_handler_declaration_replacer;
            }
            QString declare_task_list_str;
            QTextStream declare_task_list(&declare_task_list_str);
            QString send_msg_task_list_str;
            QTextStream send_msg_task_list(&send_msg_task_list_str);
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
                                  << timer_task.task_name.c_str()
                                  << ");";
                send_msg_task_list << "\t" << timer_task.task_name.c_str();
            }
            code_generator::Replacer declarations_replacer{declarations_file_name};
            declarations_replacer.add_tag("DeclareTaskList", declare_task_list.readAll())
                                 .add_tag("KeyHandlersPrototypeList", key_handlers_prototype_list.readAll())
                                 .add_tag("SendMsgTaskList", send_msg_task_list.readAll())
                                 .add_tag("KeyHandlersDeclarationList", key_handlers_declaration_list.readAll());
            declarations_replacer.replace_tags();
            out_file_stream << declarations_replacer;
            out_file_stream << global_variables_declaration.c_str() << "\n\n";
            QFile setup_func_file(setup_func_file_name);
            if(!setup_func_file.open(QFile::ReadOnly | QFile::Text))
            {
                qDebug() << "Could not open file " << setup_func_file_name;
                return;
            }
            QFile can_send_task_file(can_send_task_file_name);
            if(!can_send_task_file.open(QFile::ReadOnly | QFile::Text))
            {
                qDebug() << "Could not open file " << can_send_task_file_name;
                return;
            }
            out_file_stream << setup_func_file.readAll()
                     << can_send_task_file.readAll();
            for(const auto& timer_task : timer_tasks)
            {
                code_generator::Replacer timer_task_code_replacer{timer_task_code_file_name};
                timer_task_code_replacer.add_tag("TaskName", timer_task.task_name.c_str())
                        .add_tag("TaskInnerCode", timer_task.inner_code.c_str());
                timer_task_code_replacer.replace_tags();
                out_file_stream << timer_task_code_replacer;
            }
            for(const auto& key_task : key_tasks)
            {
                QString key_task_code_str;
                QTextStream key_task_code(&key_task_code_str);
                key_task_code << "void " << key_task.task_name.c_str()
                              << "(" << key_task.task_parameter.c_str() << ")\n{"
                              << key_task.task_inner_code.c_str() << "}\n\n";
                out_file_stream << key_task_code.readAll();
            }
            out_file_stream << util::read_resource(pins_reader_task_code_file_name);
            for(const auto& function : functions)
            {
                out_file_stream << function.c_str() << "\n\n";
            }
        }
    private:
        QString output_file_name;
        QString includes_file_name;
        QString msg_types_header_file_name;
        QString declarations_file_name;
        QString key_handler_declaration_file_name;
        QString setup_func_file_name;
        QString can_send_task_file_name;
        QString timer_task_code_file_name;
        QString pins_reader_task_code_file_name;
    };
}

#endif //CODE_GENERATOR_CPP_FILE_GENERATOR_H
