#include "ast.h"
#include "parser.h"
#include "util.h"
#include "replacer.h"
#include "oil_file_generator.h"
#include "msg_types_file_generator.h"
#include "cpp_file_generator.h"
#include "command_runner.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

int main(int argc, const char *argv[])
{
    using namespace code_generator::util;
    // argv[1] = C:\Users\Lucas Avelino\Desktop\ProjetoBusMaster1\EMS_NODE.def
    // argv[2] = C:\Users\Lucas Avelino\Desktop\ProjetoBusMaster1\New_DatabaseJ19391.DBF
    // argv[3] = oil_out.oil
    // argv[4] = ../../Arduino/trampoline
    // argv[5] = cpp_file.cpp
    // argv[6] = trampuino_busmaster
    // argv[7] = msg_types.h
    // argv[8] = C:\Users\Lucas Avelino\Desktop\ProjetoBusMaster1\EMS_Node.cpp
    // argv[9] = C:\Users\Lucas Avelino\Desktop\OutputFolder
    // argv[10] = C:\Arduino\goil-windows\goil.exe
    // argv[11] = {OnKey_i:4,OnKey_d:5}
    // argv[12] = COM6
    // argv[13] = y
    if(argc != 14)
    {
        std::cout << "Usage: " << argv[0] << " <def_file_path> <DBF_file_path> <output_oil_file_path> "
                  << "<trampoline_root_path> <output_cpp_file_path> <output_binary_file_path> "
                  << "<output_msg_types_header_file_path> <source_cpp_file_path> "
                  << "<output_folder> <goil_exe_path> <key_mapping_file_path> <COM_port> <arduino_nano(y/n)>\n\n";
        return 0;
    }
    fs::path def_file_path{argv[1]};
    fs::path dbf_file_path{argv[2]};
    std::string output_oil_file_name{argv[3]};
    fs::path trampoline_root_path{argv[4]};
    std::string output_cpp_file_name{argv[5]};
    std::string output_exe_file_name{argv[6]};
    std::string output_msg_types_header_file_name{argv[7]};
    fs::path cpp_src_file_path{argv[8]};
    fs::path output_folder_path{argv[9]};
    fs::path goil_exe_path{argv[10]};
    std::string key_mapping_str{argv[11]};
    std::string com_port{argv[12]};
    auto arduino_nano = std::string(argv[13]) == "y";

    fs::path output_oil_file_path{output_folder_path};
    output_oil_file_path /= output_oil_file_name;

    std::string trampoline_root_path_relative_to_output_folder{fs::relative(trampoline_root_path,output_folder_path).generic_string()};

    fs::path output_msg_types_header_file_path{output_folder_path};
    output_msg_types_header_file_path /= output_msg_types_header_file_name;

    fs::path output_cpp_file_path{output_folder_path};
    output_cpp_file_path /= output_cpp_file_name;

    fs::path trampoline_goil_templates_path{trampoline_root_path};
    trampoline_goil_templates_path /= "goil/templates/";

    fs::path target_header_files_folder_path{fs::current_path()};
    target_header_files_folder_path /= "target_header_files";

    fs::path J1939Includes_header_path{target_header_files_folder_path};
    J1939Includes_header_path /= "J1939Includes.h";
    fs::path J1939Includes_header_output_path{output_folder_path};
    J1939Includes_header_output_path /= "J1939Includes.h";

    fs::path message_queue_header_path{target_header_files_folder_path};
    message_queue_header_path /= "message_queue.h";
    fs::path message_queue_header_output_path{output_folder_path};
    message_queue_header_output_path /= "message_queue.h";

    fs::path windows_types_header_path{target_header_files_folder_path};
    windows_types_header_path /= "windows_types.h";
    fs::path windows_types_header_output_path{output_folder_path};
    windows_types_header_output_path /= "windows_types.h";

    auto key_mapping = get_key_mapping(key_mapping_str);

    std::vector<code_generator::ast::TimerHandler> timer_handlers;
    std::vector<code_generator::ast::KeyHandler> key_handlers;
    std::tie(timer_handlers, key_handlers) = read_def(def_file_path.string());
    auto msgs = read_dbf(dbf_file_path.string());
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
    std::tie(timer_tasks, key_tasks, functions) = get_functions(cpp_src_file_path.string(), timer_handlers, key_handlers, key_mapping);
    auto global_variables_declaration = get_global_variables_declaration(cpp_src_file_path.string());
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
    fs::remove(J1939Includes_header_output_path);
    fs::remove(message_queue_header_output_path);
    fs::remove(windows_types_header_output_path);
    fs::copy_file(J1939Includes_header_path, J1939Includes_header_output_path);
    fs::copy_file(message_queue_header_path, message_queue_header_output_path);
    fs::copy_file(windows_types_header_path, windows_types_header_output_path);
    code_generator::CommandRunner goil_cmd_run{goil_exe_path.string(), output_folder_path.string(), {"--target=avr/arduino/uno", "--templates=" + fs::relative(trampoline_goil_templates_path, output_folder_path).generic_string(), output_oil_file_name}};
    code_generator::CommandRunner make_cmd_run{"make.py", output_folder_path.string(), {}};

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
    goil_cmd_run();
    make_cmd_run();
    avrdude_cmd_run();
    return 0;
}