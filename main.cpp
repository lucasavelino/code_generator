#include "code_generator.h"

int main(int argc, const char *argv[])
{
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
    auto cd = code_generator::CodeGenerator::create()
            .set_def_file_path(argv[1])
            .set_dbf_file_path(argv[2])
            .set_output_oil_file_name(argv[3])
            .set_trampoline_root_path(argv[4])
            .set_output_cpp_file_name(argv[5])
            .set_output_exe_file_name(argv[6])
            .set_output_msg_types_header_file_name(argv[7])
            .set_cpp_src_file_path(argv[8])
            .set_output_folder_path(argv[9])
            .set_goil_exe_path(argv[10])
            .set_key_mapping(argv[11])
            .set_com_port(argv[12])
            .is_arduino_nano(std::string(argv[13]) == "y")
            .flash(false)
            .build();
    cd.execute();
    return 0;
}