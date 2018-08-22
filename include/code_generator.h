#ifndef CODE_GENERATOR_CODE_GENERATOR_H
#define CODE_GENERATOR_CODE_GENERATOR_H

#include "util.h"
#include "oil_file_generator.h"
#include "msg_types_file_generator.h"
#include "cpp_file_generator.h"
#include "command_runner.h"
#include <boost/filesystem.hpp>
#include <map>

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
            std::tie(timer_handlers, key_handlers) = util::read_def(def_file_path.string());
            auto msgs = util::read_dbf(dbf_file_path.string());
            code_generator::OilFileGenerator oil_generator{output_oil_file_path.string(),
                                                           R"(code_templates\timer_task_oil.txt)",
                                                           R"(code_templates\can_send_task_oil.txt)",
                                                           R"(code_templates\pins_reader_task_oil.txt)",
                                                           R"(code_templates\oil_ini.txt)",
                                                           R"(code_templates\oil_fim.txt)",
                                                           trampoline_root_path_relative_to_output_folder,
                                                           output_cpp_file_name,
                                                           output_exe_file_name};

            oil_generator.generate(timer_handlers, timer_handlers.size() + 1 + 1);

            code_generator::MsgTypesFileGenerator msg_types_generator{output_msg_types_header_file_path.string(),
                                                                      R"(code_templates\msg_types_header.txt)",
                                                                      R"(code_templates\db_msg_class_declaration.txt)",
                                                                      R"(code_templates\db_msg_signal_construction.txt)",
                                                                      R"(code_templates\db_msg_signal_declaration.txt)"};
            msg_types_generator.generate(msgs);

            std::vector<code_generator::util::TimerTask> timer_tasks;
            std::vector<code_generator::util::KeyTask> key_tasks;
            std::vector<std::string> functions;
            std::tie(timer_tasks, key_tasks, functions) = util::get_functions(cpp_src_file_path.string(), timer_handlers, key_handlers, key_mapping);
            auto global_variables_declaration = util::get_global_variables_declaration(cpp_src_file_path.string());
            code_generator::CppFileGenerator cpp_file_generator{output_cpp_file_path.string(),
                                                                R"(code_templates\includes.txt)",
                                                                output_msg_types_header_file_name,
                                                                R"(code_templates\declarations.txt)",
                                                                R"(code_templates\key_handler_declaration.txt)",
                                                                R"(code_templates\setup_func.txt)",
                                                                R"(code_templates\can_send_task.txt)",
                                                                R"(code_templates\timer_task_code.txt)",
                                                                R"(code_templates\pins_reader_task_code.txt)"};
            cpp_file_generator.generate(functions, timer_tasks, key_tasks, global_variables_declaration);
            boost::filesystem::remove(J1939Includes_header_output_path);
            boost::filesystem::remove(message_queue_header_output_path);
            boost::filesystem::remove(windows_types_header_output_path);
            boost::filesystem::copy_file(J1939Includes_header_path, J1939Includes_header_output_path);
            boost::filesystem::copy_file(message_queue_header_path, message_queue_header_output_path);
            boost::filesystem::copy_file(windows_types_header_path, windows_types_header_output_path);
            code_generator::CommandRunner goil_cmd_run{goil_exe_path.string(), output_folder_path.string(), {"--target=avr/arduino/uno", "--templates=" + boost::filesystem::relative(trampoline_goil_templates_path, output_folder_path).generic_string(), output_oil_file_name}};
            code_generator::CommandRunner make_cmd_run{"make.py", output_folder_path.string(), {}};

            goil_cmd_run();
            make_cmd_run();
            if(flash)
            {
                std::vector<std::string> avrdude_parameters;
                avrdude_parameters.emplace_back("-c arduino");
                avrdude_parameters.emplace_back("-p m328p");
                avrdude_parameters.push_back("-P " + com_port);
                if(arduino_nano)
                {
                    avrdude_parameters.emplace_back("-b57600");
                }
                std::stringstream flash_parameter;
                flash_parameter << "-U flash:w:" << output_exe_file_name << ".hex";
                avrdude_parameters.push_back(flash_parameter.str());

                code_generator::CommandRunner avrdude_cmd_run{"avrdude", output_folder_path.string(), avrdude_parameters};
                avrdude_cmd_run();
            }
        }
    private:
        CodeGenerator() = default;
        boost::filesystem::path def_file_path;
        boost::filesystem::path dbf_file_path;
        boost::filesystem::path output_oil_file_path;
        boost::filesystem::path output_cpp_file_path;
        boost::filesystem::path trampoline_root_path;
        boost::filesystem::path cpp_src_file_path;
        boost::filesystem::path output_folder_path;
        boost::filesystem::path goil_exe_path;
        boost::filesystem::path output_msg_types_header_file_path;
        boost::filesystem::path trampoline_goil_templates_path;
        boost::filesystem::path target_header_files_folder_path;
        boost::filesystem::path J1939Includes_header_path;
        boost::filesystem::path J1939Includes_header_output_path;
        boost::filesystem::path message_queue_header_path;
        boost::filesystem::path message_queue_header_output_path;
        boost::filesystem::path windows_types_header_path;
        boost::filesystem::path windows_types_header_output_path;
        std::string output_oil_file_name;
        std::string output_cpp_file_name;
        std::string output_exe_file_name;
        std::string output_msg_types_header_file_name;
        std::string trampoline_root_path_relative_to_output_folder;
        std::map<std::string, unsigned int> key_mapping;
        std::string com_port;
        bool arduino_nano{};
        bool flash{};
        friend class CodeGeneratorBuilder;
    };

    class CodeGeneratorBuilder
    {
    public:
        CodeGeneratorBuilder& set_dbf_file_path(const std::string &dbf_file_path)
        {
            cd.dbf_file_path = dbf_file_path;
            return *this;
        }

        CodeGeneratorBuilder& set_def_file_path(const std::string &def_file_path)
        {
            cd.def_file_path = def_file_path;
            return *this;
        }

        CodeGeneratorBuilder& set_output_oil_file_name(std::string output_oil_file_name)
        {
            cd.output_oil_file_name = std::move(output_oil_file_name);
            return *this;
        }

        CodeGeneratorBuilder& set_trampoline_root_path(const std::string& trampoline_root_path)
        {
            cd.trampoline_root_path = boost::filesystem::path(trampoline_root_path);
            return *this;
        }

        CodeGeneratorBuilder& set_output_cpp_file_name(std::string output_cpp_file_name)
        {
            cd.output_cpp_file_name = std::move(output_cpp_file_name);
            return *this;
        }

        CodeGeneratorBuilder& set_output_exe_file_name(std::string output_exe_file_name)
        {
            cd.output_exe_file_name = std::move(output_exe_file_name);
            return *this;
        }

        CodeGeneratorBuilder& set_output_msg_types_header_file_name(std::string output_msg_types_header_file_name)
        {
            cd.output_msg_types_header_file_name = std::move(output_msg_types_header_file_name);
            return *this;
        }

        CodeGeneratorBuilder& set_cpp_src_file_path(const std::string& cpp_src_file_path)
        {
            cd.cpp_src_file_path = cpp_src_file_path;
            return *this;
        }

        CodeGeneratorBuilder& set_output_folder_path(const std::string& output_folder_path)
        {
            cd.output_folder_path = output_folder_path;
            return *this;
        }

        CodeGeneratorBuilder& set_goil_exe_path(const std::string& goil_exe_path)
        {
            cd.goil_exe_path = goil_exe_path;
            return *this;
        }

        CodeGeneratorBuilder& set_key_mapping(const std::string& key_mapping_str)
        {
            cd.key_mapping = util::get_key_mapping(key_mapping_str);
            return *this;
        }

        CodeGeneratorBuilder& set_com_port(std::string com_port)
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
            cd.output_oil_file_path = cd.output_folder_path / cd.output_oil_file_name;
            cd.trampoline_root_path_relative_to_output_folder = boost::filesystem::relative(cd.trampoline_root_path,cd.output_folder_path).generic_string();
            cd.output_msg_types_header_file_path = cd.output_folder_path / cd.output_msg_types_header_file_name;
            cd.output_cpp_file_path = cd.output_folder_path / cd.output_cpp_file_name;
            cd.trampoline_goil_templates_path = cd.trampoline_root_path / "goil/templates/";
            cd.target_header_files_folder_path = boost::filesystem::current_path() / "target_header_files";
            cd.J1939Includes_header_path = cd.target_header_files_folder_path / "J1939Includes.h";
            cd.J1939Includes_header_output_path = cd.output_folder_path / "J1939Includes.h";
            cd.message_queue_header_path = cd.target_header_files_folder_path / "message_queue.h";
            cd.message_queue_header_output_path = cd.output_folder_path / "message_queue.h";
            cd.windows_types_header_path = cd.target_header_files_folder_path / "windows_types.h";
            cd.windows_types_header_output_path = cd.output_folder_path / "windows_types.h";
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
