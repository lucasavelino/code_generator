#ifndef CODE_GENERATOR_CODE_GENERATOR_H
#define CODE_GENERATOR_CODE_GENERATOR_H

#include "oil_file_generator.h"
#include "msg_types_file_generator.h"
#include "cpp_file_generator.h"
#include "command_runner.h"
#include "util.h"
#include <map>
#include <QString>
#include <QList>
#include <QDir>

namespace code_generator
{
    class CodeGenerator
    {
    public:
        CodeGenerator() = default;
        QString execute_build() const
        {
            std::vector<code_generator::ast::TimerHandler> timer_handlers;
            std::vector<code_generator::ast::KeyHandler> key_handlers;
            code_generator::ast::MessageHandlerPgnAll msg_handler_pgn_all;
            std::tie(timer_handlers, key_handlers, msg_handler_pgn_all) = util::read_def(def_file_path.toStdString());
            auto msgs = util::read_dbf(dbf_file_path.toStdString());
            code_generator::OilFileGenerator oil_generator{output_oil_file_path,
                                                           R"(:/code_templates/timer_task_oil.txt)",
                                                           R"(:/code_templates/can_send_task_oil.txt)",
                                                           R"(:/code_templates/pins_reader_task_oil.txt)",
                                                           R"(:/code_templates/oil_ini.txt)",
                                                           R"(:/code_templates/oil_fim.txt)",
                                                           trampoline_root_path_relative_to_output_folder,
                                                           output_cpp_file_name,
                                                           output_exe_file_name};

            oil_generator.generate(timer_handlers, timer_handlers.size() + 1 + 1);

            code_generator::MsgTypesFileGenerator msg_types_generator{output_msg_types_header_file_path,
                                                                      R"(:/code_templates/msg_types_header.txt)",
                                                                      R"(:/code_templates/db_msg_class_declaration.txt)",
                                                                      R"(:/code_templates/db_msg_signal_construction.txt)",
                                                                      R"(:/code_templates/db_msg_signal_declaration.txt)"};
            msg_types_generator.generate(msgs);

            std::vector<code_generator::util::TimerTask> timer_tasks;
            std::vector<code_generator::util::KeyTask> key_tasks;
            code_generator::util::PgnAllTask pgn_all_task;
            std::vector<std::string> functions;
            bool error_on_key_mapping{false};
            QString error_on_key_mapping_str;
            std::tie(timer_tasks, key_tasks, pgn_all_task, functions, error_on_key_mapping_str) = util::get_functions(cpp_src_file_path.toStdString(), timer_handlers, key_handlers, key_mapping, msg_handler_pgn_all, error_on_key_mapping);
            if(error_on_key_mapping)
            {
                return error_on_key_mapping_str;
            }
            auto global_variables_declaration = util::get_global_variables_declaration(cpp_src_file_path.toStdString());
            code_generator::CppFileGenerator cpp_file_generator{output_cpp_file_path,
                                                                R"(:/code_templates/includes.txt)",
                                                                output_msg_types_header_file_name,
                                                                R"(:/code_templates/declarations.txt)",
                                                                R"(:/code_templates/key_handler_declaration.txt)",
                                                                R"(:/code_templates/setup_func.txt)",
                                                                R"(:/code_templates/can_send_task.txt)",
                                                                R"(:/code_templates/timer_task_code.txt)",
                                                                R"(:/code_templates/pins_reader_task_code.txt)"};
            cpp_file_generator.generate(functions, timer_tasks, key_tasks, global_variables_declaration);
            QFile(J1939Includes_header_output_path).remove();
            QFile(message_queue_header_output_path).remove();
            QFile(windows_types_header_output_path).remove();
            QFile(mutual_exclusion_handler_trampoline_header_output_path).remove();
            QFile(R"(:/target_header_files/J1939Includes.h)").copy(J1939Includes_header_output_path);
            QFile(R"(:/target_header_files/message_queue.h)").copy(message_queue_header_output_path);
            QFile(R"(:/target_header_files/windows_types.h)").copy(windows_types_header_output_path);
            QFile(R"(:/target_header_files/mutual_exclusion_handler_trampoline.h)").copy(mutual_exclusion_handler_trampoline_header_output_path);
            QFile::setPermissions(J1939Includes_header_output_path, QFile::WriteOwner | QFile::ReadOwner
                                  | QFile::WriteUser | QFile::ReadUser | QFile::WriteGroup | QFile::ReadGroup
                                  | QFile::WriteOther | QFile::ReadOther);
            QFile::setPermissions(message_queue_header_output_path, QFile::WriteOwner | QFile::ReadOwner
                                  | QFile::WriteUser | QFile::ReadUser | QFile::WriteGroup | QFile::ReadGroup
                                  | QFile::WriteOther | QFile::ReadOther);
            QFile::setPermissions(windows_types_header_output_path, QFile::WriteOwner | QFile::ReadOwner
                                  | QFile::WriteUser | QFile::ReadUser | QFile::WriteGroup | QFile::ReadGroup
                                  | QFile::WriteOther | QFile::ReadOther);
            QFile::setPermissions(mutual_exclusion_handler_trampoline_header_output_path, QFile::WriteOwner | QFile::ReadOwner
                                  | QFile::WriteUser | QFile::ReadUser | QFile::WriteGroup | QFile::ReadGroup
                                  | QFile::WriteOther | QFile::ReadOther);

            code_generator::CommandRunner goil_cmd_run(goil_exe_path, {"--target=avr/arduino/uno", QString("--templates=") + QDir(output_folder_path).relativeFilePath(trampoline_goil_templates_path), output_oil_file_name});
            goil_cmd_run.set_directory(output_folder_path);
            code_generator::CommandRunner make_cmd_run("python", {QDir(output_folder_path).absoluteFilePath("make.py")});
            make_cmd_run.set_directory(output_folder_path);

            QString output_str;
            QTextStream output(&output_str);

            goil_cmd_run();
            make_cmd_run();

            output << goil_cmd_run.get_output() << make_cmd_run.get_output();
            return output.readAll();
        }

        QString execute_flash() const
        {
            code_generator::CommandRunner avrdude_cmd_run{"avrdude"};
            avrdude_cmd_run.add_parameter("-carduino")
                           .add_parameter("-pm328p")
                           .add_parameter("-P" + com_port);
            if(arduino_nano)
            {
                avrdude_cmd_run.add_parameter("-b57600");
            }
            avrdude_cmd_run.add_parameter(QString("-Uflash:w:") + output_exe_file_name + ".hex");

            avrdude_cmd_run.set_directory(output_folder_path);
            avrdude_cmd_run();

            QString output_str;
            QTextStream output(&output_str);

            output << avrdude_cmd_run.get_output();
            return output.readAll();
        }

        QString execute() const
        {
            QString out;
            QTextStream text_stream(&out);
            text_stream << execute_build();
            if(flash)
            {
                text_stream << execute_flash();
            }
            return text_stream.readAll();
        }

    private:
        QString def_file_path;
        QString dbf_file_path;
        QString output_oil_file_path;
        QString output_cpp_file_path;
        QString trampoline_root_path;
        QString cpp_src_file_path;
        QString output_folder_path;
        QString goil_exe_path;
        QString output_msg_types_header_file_path;
        QString trampoline_goil_templates_path;
        QString J1939Includes_header_output_path;
        QString message_queue_header_output_path;
        QString windows_types_header_output_path;
        QString mutual_exclusion_handler_trampoline_header_output_path;
        QString output_oil_file_name;
        QString output_cpp_file_name;
        QString output_exe_file_name;
        QString output_msg_types_header_file_name;
        QString trampoline_root_path_relative_to_output_folder;
        std::map<std::string, unsigned int> key_mapping;
        QString com_port;
        bool arduino_nano{};
        bool flash{};
        friend class CodeGeneratorPropertiesManager;
    };

    class CodeGeneratorPropertiesManager
    {
    public:
        CodeGeneratorPropertiesManager(CodeGenerator& cd)
            : cd{cd}
        {}

        CodeGeneratorPropertiesManager& set_dbf_file_path(QString dbf_file_path)
        {
            cd.dbf_file_path = std::move(dbf_file_path);
            return *this;
        }

        CodeGeneratorPropertiesManager& set_def_file_path(QString def_file_path)
        {
            cd.def_file_path = std::move(def_file_path);
            return *this;
        }

        CodeGeneratorPropertiesManager& set_output_oil_file_name(QString output_oil_file_name)
        {
            cd.output_oil_file_name = std::move(output_oil_file_name);
            return *this;
        }

        CodeGeneratorPropertiesManager& set_trampoline_root_path(QString trampoline_root_path)
        {
            cd.trampoline_root_path = std::move(trampoline_root_path);
            return *this;
        }

        CodeGeneratorPropertiesManager& set_output_cpp_file_name(QString output_cpp_file_name)
        {
            cd.output_cpp_file_name = std::move(output_cpp_file_name);
            return *this;
        }

        CodeGeneratorPropertiesManager& set_output_exe_file_name(QString output_exe_file_name)
        {
            cd.output_exe_file_name = std::move(output_exe_file_name);
            return *this;
        }

        CodeGeneratorPropertiesManager& set_output_msg_types_header_file_name(QString output_msg_types_header_file_name)
        {
            cd.output_msg_types_header_file_name = std::move(output_msg_types_header_file_name);
            return *this;
        }

        CodeGeneratorPropertiesManager& set_cpp_src_file_path(QString cpp_src_file_path)
        {
            cd.cpp_src_file_path = std::move(cpp_src_file_path);
            return *this;
        }

        CodeGeneratorPropertiesManager& set_output_folder_path(QString output_folder_path)
        {
            cd.output_folder_path = std::move(output_folder_path);
            return *this;
        }

        CodeGeneratorPropertiesManager& set_goil_exe_path(QString goil_exe_path)
        {
            cd.goil_exe_path = std::move(goil_exe_path);
            return *this;
        }

        CodeGeneratorPropertiesManager& set_key_mapping(const std::string& key_mapping_str)
        {
            cd.key_mapping = util::get_key_mapping(key_mapping_str);
            return *this;
        }

        CodeGeneratorPropertiesManager& set_com_port(QString com_port)
        {
            cd.com_port = std::move(com_port);
            return *this;
        }

        CodeGeneratorPropertiesManager& is_arduino_nano(bool arduino_nano)
        {
            cd.arduino_nano = arduino_nano;
            return *this;
        }

        CodeGeneratorPropertiesManager& flash(bool flash)
        {
            cd.flash = flash;
            return *this;
        }

        CodeGeneratorPropertiesManager& configure()
        {
            if(cd.output_folder_path.isEmpty())
            {
                qDebug() << "Error: Path to output folder is empty.\n";
                return *this;
            } else
            {
                cd.J1939Includes_header_output_path = QDir(cd.output_folder_path).absoluteFilePath("J1939Includes.h");
                cd.message_queue_header_output_path = QDir(cd.output_folder_path).absoluteFilePath("message_queue.h");
                cd.windows_types_header_output_path = QDir(cd.output_folder_path).absoluteFilePath("windows_types.h");
                cd.mutual_exclusion_handler_trampoline_header_output_path = QDir(cd.output_folder_path).absoluteFilePath("mutual_exclusion_handler_trampoline.h");
                if(cd.output_oil_file_name.isEmpty())
                {
                    qDebug() << "Error: Output oil file name is empty\n";
                    return *this;
                }
                cd.output_oil_file_path = QDir(cd.output_folder_path).absoluteFilePath(cd.output_oil_file_name);
                if(cd.trampoline_root_path.isEmpty())
                {
                    qDebug() << "Error: Trampoline root path is empty\n";
                    return *this;
                }
                cd.trampoline_root_path_relative_to_output_folder = QDir(cd.output_folder_path).relativeFilePath(cd.trampoline_root_path);
                if(cd.output_msg_types_header_file_name.isEmpty())
                {
                    qDebug() << "Error: Output message types header file name is empty\n";
                    return *this;
                }
                cd.output_msg_types_header_file_path = QDir(cd.output_folder_path).absoluteFilePath(cd.output_msg_types_header_file_name);
                if(cd.output_cpp_file_name.isEmpty())
                {
                    qDebug() << "Error: Output cpp file name is empty\n";
                    return *this;
                }
                cd.output_cpp_file_path = QDir(cd.output_folder_path).absoluteFilePath(cd.output_cpp_file_name);
            }

            if(cd.trampoline_root_path.isEmpty())
            {
                qDebug() << "Error: Trampoline root path is empty\n";
                return *this;
            }
            cd.trampoline_goil_templates_path = QDir(cd.trampoline_root_path).absoluteFilePath("goil/templates/");
            return *this;
        }
    private:
        CodeGenerator& cd;
    };
}

#endif //CODE_GENERATOR_CODE_GENERATOR_H
