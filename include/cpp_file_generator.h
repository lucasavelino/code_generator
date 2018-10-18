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
                         QString digital_key_handler_declaration_file_name,
                         QString analog_key_handler_declaration_file_name,
                         QString setup_func_file_name, QString can_send_task_file_name,
                         QString can_recv_task_file_name,
                         QString timer_task_code_file_name, QString pins_reader_task_code_file_name)
                : output_file_name(std::move(output_file_name)), includes_file_name(std::move(includes_file_name)),
                  msg_types_header_file_name(std::move(msg_types_header_file_name)), declarations_file_name(std::move(declarations_file_name)),
                  digital_key_handler_declaration_file_name(std::move(digital_key_handler_declaration_file_name)),
                  analog_key_handler_declaration_file_name(std::move(analog_key_handler_declaration_file_name)),
                  setup_func_file_name(std::move(setup_func_file_name)), can_send_task_file_name(std::move(can_send_task_file_name)),
                  can_recv_task_file_name(std::move(can_recv_task_file_name)),
                  timer_task_code_file_name(std::move(timer_task_code_file_name)), pins_reader_task_code_file_name(std::move(pins_reader_task_code_file_name))
        {}

        void generate(const std::vector<std::string>& functions,
                      const std::vector<code_generator::util::TimerTask>& timer_tasks,
                      const std::vector<code_generator::util::KeyTask>& key_tasks,
                      const std::string& global_variables_declaration,
                      const util::PgnAllTask& pgn_all_task,
                      const util::SystemTasksInfo& tasks_info)
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
            QString digital_key_handlers_prototype_list_str;
            QString digital_key_handlers_declaration_list_str;
            QString analog_key_handlers_prototype_list_str;
            QString analog_key_handlers_declaration_list_str;
            QTextStream digital_key_handlers_prototype_list(&digital_key_handlers_prototype_list_str);
            QTextStream digital_key_handlers_declaration_list(&digital_key_handlers_declaration_list_str);
            QTextStream analog_key_handlers_prototype_list(&analog_key_handlers_prototype_list_str);
            QTextStream analog_key_handlers_declaration_list(&analog_key_handlers_declaration_list_str);
            auto none_digital_key_task_yet = true;
            auto none_analog_key_task_yet = true;
            for(const auto& key_task : key_tasks)
            {
                if(key_task.digital)
                {
                    if(none_digital_key_task_yet)
                    {
                        none_digital_key_task_yet = false;
                    } else
                    {
                        digital_key_handlers_prototype_list << "\n";
                        digital_key_handlers_declaration_list << ",\n";
                    }
                    Replacer digital_key_handler_declaration_replacer{digital_key_handler_declaration_file_name};
                    digital_key_handler_declaration_replacer.add_tag("DigitalInputPin", QString::number(key_task.pin))
                                                            .add_tag("ActiveLevel", key_task.active_state_high ? QString("HIGH") : QString("LOW"))
                                                            .add_tag("KeyHandler", key_task.task_name.c_str());
                    digital_key_handler_declaration_replacer.replace_tags();
                    digital_key_handlers_prototype_list << "void " << key_task.task_name.c_str() << "(" << key_task.task_parameter.c_str() << ");";
                    digital_key_handlers_declaration_list << "\t" << digital_key_handler_declaration_replacer;
                } else
                {
                    if(none_analog_key_task_yet)
                    {
                        none_analog_key_task_yet = false;
                    } else
                    {
                        analog_key_handlers_prototype_list << "\n";
                        analog_key_handlers_declaration_list << ",\n";
                    }
                    Replacer analog_key_handler_declaration_replacer{analog_key_handler_declaration_file_name};
                    analog_key_handler_declaration_replacer.add_tag("AnalogInputPin", QString::number(key_task.pin))
                                                           .add_tag("KeyHandler", key_task.task_name.c_str());
                    analog_key_handler_declaration_replacer.replace_tags();
                    analog_key_handlers_prototype_list << "void " << key_task.task_name.c_str() << "(unsigned int KeyValue);";
                    analog_key_handlers_declaration_list << "\t" << analog_key_handler_declaration_replacer;
                }
            }
            QString declare_task_list_str;
            QTextStream declare_task_list(&declare_task_list_str);
            QString send_msg_task_list_str;
            QTextStream send_msg_task_list(&send_msg_task_list_str);
            QString message_handler_prototype_list_str;
            QTextStream message_handler_prototype_list(&message_handler_prototype_list_str);
            if(tasks_info.pins_reader_task_used)
            {
                declare_task_list << "DeclareTask(pins_reader);\n";
                send_msg_task_list << "\tpins_reader,\n";
            }
            if(tasks_info.can_send_task_used)
            {
                declare_task_list << "DeclareTask(can_send_task);\n";
            }
            if(tasks_info.can_recv_task_used)
            {
                declare_task_list << "DeclareTask(can_recv_task);\n";
                send_msg_task_list << "\tcan_recv_task"
                                   << (tasks_info.timer_task_used ? ",\n" : "\n");
                if(tasks_info.message_handler_pgn_all_used)
                {
                    message_handler_prototype_list << "void OnPGN_All(J1939_MSG&);\n";
                }
            }
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
            declarations_replacer.add_tag("DeclareCanSendResourcesAndEvents",
                                          tasks_info.can_send_task_used ?
                                              "DeclareResource(can_send_msg_queue_resource);\n"
                                              "DeclareEvent(can_send_msg_queue_empty);"
                                              "\nDeclareEvent(can_send_msg_queue_full);\n" :
                                              "")
                                 .add_tag("DeclareCanRecvResourcesAndEvents",
                                          tasks_info.can_recv_task_used ?
                                              "DeclareEvent(can_recv_evt);\n" :
                                              "")
                                 .add_tag("DeclareTaskList", declare_task_list.readAll())
                                 .add_tag("DigitalKeyHandlersPrototypeList", digital_key_handlers_prototype_list.readAll())
                                 .add_tag("AnalogKeyHandlersPrototypeList", analog_key_handlers_prototype_list.readAll())
                                 .add_tag("MessageHandlersPrototypeList", message_handler_prototype_list.readAll())
                                 .add_tag("SendMsgTaskList", send_msg_task_list.readAll())
                                 .add_tag("SendMsgReceiver", tasks_info.can_send_task_used ?
                                                                "	can_send_task" :
                                                                "")
                                 .add_tag("InstantiateCanSendDataStructures",
                                          tasks_info.can_send_task_used ?
                                              "MutualExclusionHandlerTrampoline<N_SENDERS,N_RECEIVERS> "
                                              "can_send_msg_mutual_exclusion_handler("
                                              "can_send_msg_queue_resource, can_send_msg_queue_empty, "
                                              "can_send_msg_queue_full, can_send_msg_queue_senders, "
                                              "can_send_msg_queue_receivers);\n"
                                              "MessageQueue< J1939_MSG, MutualExclusionHandlerTrampoline<N_SENDERS, N_RECEIVERS> > "
                                              "can_send_msg_queue(can_send_msg_mutual_exclusion_handler);" :
                                              "")
                                 .add_tag("DigitalKeyHandlersDeclarationList", digital_key_handlers_declaration_list.readAll())
                                 .add_tag("AnalogKeyHandlersDeclarationList", analog_key_handlers_declaration_list.readAll())
                                 .add_tag("SendMsgCode",
                                          tasks_info.can_send_task_used ?
                                              "\tcan_send_msg_queue.send(msg);" :
                                              "");
            declarations_replacer.replace_tags();
            out_file_stream << declarations_replacer;
            out_file_stream << global_variables_declaration.c_str() << "\n\n";
            Replacer setup_func_replacer{setup_func_file_name};
            setup_func_replacer.add_tag("SerialInit", tasks_info.serial_used ? "Serial.begin(115200);" : "")
                               .add_tag("InitializeDigitalInputPins",
                                        (tasks_info.pins_reader_task_used ?
                                         "\tfor(unsigned int i = 0; i < N_DIGITAL_KEY_HANDLERS; ++i)\n"
                                         "\t{\n"
                                         "\t\tpinMode(digital_key_handlers[i].key, INPUT);\n"
                                         "\t}" :
                                         ""));
            setup_func_replacer.replace_tags();
            out_file_stream << setup_func_replacer;
            QFile can_send_task_file(can_send_task_file_name);
            if(!can_send_task_file.open(QFile::ReadOnly | QFile::Text))
            {
                qDebug() << "Could not open file " << can_send_task_file_name;
                return;
            }
            if(tasks_info.can_send_task_used)
            {
                out_file_stream << can_send_task_file.readAll();
            }
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
                              << "("
                              << (key_task.digital ? key_task.task_parameter.c_str() : "unsigned int KeyValue")
                              << ")\n{\n"
                              << key_task.task_inner_code.c_str() << "}\n\n";
                out_file_stream << key_task_code.readAll();
            }
            if(tasks_info.pins_reader_task_used)
            {
                QFile pins_reader_task_code_file{pins_reader_task_code_file_name};
                if(!pins_reader_task_code_file.open(QFile::ReadOnly | QFile::Text))
                {
                    qDebug() << "Could not open file " << pins_reader_task_code_file_name;
                    return;
                }
                out_file_stream << pins_reader_task_code_file.readAll();
            }
            if(tasks_info.can_recv_task_used)
            {
                if(tasks_info.message_handler_pgn_all_used)
                {
                    QString msg_handler_pgn_all_code_str;
                    QTextStream msg_handler_pgn_all_code(&msg_handler_pgn_all_code_str);
                    msg_handler_pgn_all_code << "void " << pgn_all_task.task_name.c_str()
                                             << "(J1939_MSG& RxMsg)\n{\n"
                                             << pgn_all_task.task_inner_code.c_str() << "}\n\n";
                    out_file_stream << msg_handler_pgn_all_code.readAll();
                }
                Replacer can_recv_task_replacer{can_recv_task_file_name};
                can_recv_task_replacer.add_tag("MessageHandlerPgnAll",
                                               (tasks_info.message_handler_pgn_all_used ?
                                                    "OnPGN_All(received_msg);" : ""));
                can_recv_task_replacer.replace_tags();
                out_file_stream << can_recv_task_replacer;
            }
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
        QString digital_key_handler_declaration_file_name;
        QString analog_key_handler_declaration_file_name;
        QString setup_func_file_name;
        QString can_send_task_file_name;
        QString can_recv_task_file_name;
        QString timer_task_code_file_name;
        QString pins_reader_task_code_file_name;
    };
}

#endif //CODE_GENERATOR_CPP_FILE_GENERATOR_H
