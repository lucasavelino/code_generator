#ifndef CODE_GENERATOR_CODE_GENERATOR_H
#define CODE_GENERATOR_CODE_GENERATOR_H

#include "util.h"
#include "oil_file_generator.h"
#include "msg_types_file_generator.h"
#include "cpp_file_generator.h"
#include "command_runner.h"
#include <map>
#include <QString>
#include <QList>
#include <QDir>

namespace code_generator
{
    class CodeGenerator;
    class CodeGeneratorBuilder;

    class CodeGenerator
    {
    public:
        static CodeGeneratorBuilder create();

        void execute() const
        {
            std::vector<code_generator::ast::TimerHandler> timer_handlers;
            std::vector<code_generator::ast::KeyHandler> key_handlers;
            std::tie(timer_handlers, key_handlers) = util::read_def(def_file_path.toStdString());
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
            std::vector<std::string> functions;
            std::tie(timer_tasks, key_tasks, functions) = util::get_functions(cpp_src_file_path.toStdString(), timer_handlers, key_handlers, key_mapping);
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
            QFile(R"(:/target_header_files/J1939Includes.h)").copy(J1939Includes_header_output_path);
            QFile(R"(:/target_header_files/message_queue.h)").copy(message_queue_header_output_path);
            QFile(R"(:/target_header_files/windows_types.h)").copy(windows_types_header_output_path);

            code_generator::CommandRunner goil_cmd_run(goil_exe_path, {"--target=avr/arduino/uno", QString("--templates=") + QDir(output_folder_path).relativeFilePath(trampoline_goil_templates_path), output_oil_file_name});
            goil_cmd_run.set_directory(output_folder_path);
            code_generator::CommandRunner make_cmd_run("python", {QDir(output_folder_path).absoluteFilePath("make.py")});
            make_cmd_run.set_directory(output_folder_path);
            goil_cmd_run();
            make_cmd_run();
            if(flash)
            {
                code_generator::CommandRunner avrdude_cmd_run{"avrdude"};
                avrdude_cmd_run.add_parameter("-c arduino")
                               .add_parameter("-p m328p")
                               .add_parameter("-P " + com_port);
                if(arduino_nano)
                {
                    avrdude_cmd_run.add_parameter("-b57600");
                }
                avrdude_cmd_run.add_parameter(QString("-U flash:w:") + output_exe_file_name + ".hex");

                avrdude_cmd_run.set_directory(output_folder_path);
                avrdude_cmd_run();
            }
        }
    private:
        CodeGenerator() = default;
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
        QString output_oil_file_name;
        QString output_cpp_file_name;
        QString output_exe_file_name;
        QString output_msg_types_header_file_name;
        QString trampoline_root_path_relative_to_output_folder;
        std::map<std::string, unsigned int> key_mapping;
        QString com_port;
        bool arduino_nano{};
        bool flash{};
        friend class CodeGeneratorBuilder;
    };

    class CodeGeneratorBuilder
    {
    public:
        CodeGeneratorBuilder& set_dbf_file_path(const QString &dbf_file_path)
        {
            cd.dbf_file_path = dbf_file_path;
            return *this;
        }

        CodeGeneratorBuilder& set_def_file_path(const QString &def_file_path)
        {
            cd.def_file_path = def_file_path;
            return *this;
        }

        CodeGeneratorBuilder& set_output_oil_file_name(QString output_oil_file_name)
        {
            cd.output_oil_file_name = std::move(output_oil_file_name);
            return *this;
        }

        CodeGeneratorBuilder& set_trampoline_root_path(const QString& trampoline_root_path)
        {
            cd.trampoline_root_path = trampoline_root_path;
            return *this;
        }

        CodeGeneratorBuilder& set_output_cpp_file_name(QString output_cpp_file_name)
        {
            cd.output_cpp_file_name = std::move(output_cpp_file_name);
            return *this;
        }

        CodeGeneratorBuilder& set_output_exe_file_name(QString output_exe_file_name)
        {
            cd.output_exe_file_name = std::move(output_exe_file_name);
            return *this;
        }

        CodeGeneratorBuilder& set_output_msg_types_header_file_name(QString output_msg_types_header_file_name)
        {
            cd.output_msg_types_header_file_name = std::move(output_msg_types_header_file_name);
            return *this;
        }

        CodeGeneratorBuilder& set_cpp_src_file_path(const QString& cpp_src_file_path)
        {
            cd.cpp_src_file_path = cpp_src_file_path;
            return *this;
        }

        CodeGeneratorBuilder& set_output_folder_path(const QString& output_folder_path)
        {
            cd.output_folder_path = output_folder_path;
            return *this;
        }

        CodeGeneratorBuilder& set_goil_exe_path(const QString& goil_exe_path)
        {
            cd.goil_exe_path = goil_exe_path;
            return *this;
        }

        CodeGeneratorBuilder& set_key_mapping(const std::string& key_mapping_str)
        {
            cd.key_mapping = util::get_key_mapping(key_mapping_str);
            return *this;
        }

        CodeGeneratorBuilder& set_com_port(QString com_port)
        {
            cd.com_port = std::move(com_port);
            return *this;
        }

        CodeGeneratorBuilder& is_arduino_nano(bool arduino_nano)
        {
            cd.arduino_nano = arduino_nano;
            return *this;
        }

        CodeGeneratorBuilder& flash(bool flash)
        {
            cd.flash = flash;
            return *this;
        }

        CodeGenerator build()
        {
            cd.output_oil_file_path = QDir(cd.output_folder_path).absoluteFilePath(cd.output_oil_file_name);
            cd.trampoline_root_path_relative_to_output_folder = QDir(cd.output_folder_path).relativeFilePath(cd.trampoline_root_path);
            cd.output_msg_types_header_file_path = QDir(cd.output_folder_path).absoluteFilePath(cd.output_msg_types_header_file_name);
            cd.output_cpp_file_path = QDir(cd.output_folder_path).absoluteFilePath(cd.output_cpp_file_name);
            cd.trampoline_goil_templates_path = QDir(cd.trampoline_root_path).absoluteFilePath("goil/templates/");
            cd.J1939Includes_header_output_path = QDir(cd.output_folder_path).absoluteFilePath("J1939Includes.h");
            cd.message_queue_header_output_path = QDir(cd.output_folder_path).absoluteFilePath("message_queue.h");
            cd.windows_types_header_output_path = QDir(cd.output_folder_path).absoluteFilePath("windows_types.h");
            return std::move(cd);
        }
    private:
        CodeGeneratorBuilder() = default;
        CodeGenerator cd;
        friend class CodeGenerator;
    };

    CodeGeneratorBuilder CodeGenerator::create()
    {
        return CodeGeneratorBuilder{};
    }
}

#endif //CODE_GENERATOR_CODE_GENERATOR_H
