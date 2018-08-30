#include "code_generator_wizard.h"
#include <QApplication>
#include "include/code_generator.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto cd = code_generator::CodeGenerator::create()
            .set_def_file_path("C:\\Users\\Lucas Avelino\\Desktop\\ProjetoBusMaster1\\EMS_NODE.def")
            .set_dbf_file_path("C:\\Users\\Lucas Avelino\\Desktop\\ProjetoBusMaster1\\New_DatabaseJ19391.DBF")
            .set_output_oil_file_name("oil_out.oil")
            .set_trampoline_root_path("C:\\Arduino\\trampoline")
            .set_output_cpp_file_name("cpp_file.cpp")
            .set_output_exe_file_name("trampuino_busmaster")
            .set_output_msg_types_header_file_name("msg_types.h")
            .set_cpp_src_file_path("C:\\Users\\Lucas Avelino\\Desktop\\ProjetoBusMaster1\\EMS_Node.cpp")
            .set_output_folder_path("C:\\Users\\Lucas Avelino\\Desktop\\OutputFolder")
            .set_goil_exe_path("C:\\Arduino\\goil-windows\\goil.exe")
            .set_key_mapping("{OnKey_i:4,OnKey_d:5}")
            .set_com_port("COM6")
            .is_arduino_nano(true)
            .flash(false)
            .build();
    cd.execute();


    CodeGeneratorWizard w;
    w.show();
    return a.exec();
}
