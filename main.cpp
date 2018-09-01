#include "code_generator_wizard.h"
#include <QApplication>
#include <QDebug>
#include <QCommandLineParser>
#include <QFileInfo>
#include <code_generator.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Code Generator");
    QApplication::setApplicationVersion("0.1");

    QCommandLineParser command_line_parser;

    command_line_parser.setApplicationDescription("This app generates arduino/trampolineRTOS source files from Busmaster generated code, builds them and flash them into an arduino UNO/NANO board");
    command_line_parser.addHelpOption();
    command_line_parser.addVersionOption();

    QCommandLineOption gui_option(
                QStringList() << "g" << "gui",
                QApplication::translate("main", "Use graphical interface")
                );
    command_line_parser.addOption(gui_option);

    QCommandLineOption cpp_src_option(
                QStringList() << "c" << "cppsrc",
                QApplication::translate("main", "Specify the cpp source file path"),
                QApplication::translate("main", "cppsrc")
                );
    command_line_parser.addOption(cpp_src_option);

    QCommandLineOption def_option(
                QStringList() << "d" << "def",
                QApplication::translate("main", "Specify the def source file path"),
                QApplication::translate("main", "def")
                );
    command_line_parser.addOption(def_option);

    QCommandLineOption dbf_option(
                QStringList() << "b" << "DBF",
                QApplication::translate("main", "Specify the DBF source file path"),
                QApplication::translate("main", "DBF")
                );
    command_line_parser.addOption(dbf_option);

    QCommandLineOption trampoline_option(
                QStringList() << "t" << "trampolineroot",
                QApplication::translate("main", "Specify the trampolineRTOS root file path"),
                QApplication::translate("main", "trampolineroot")
                );
    command_line_parser.addOption(trampoline_option);

    QCommandLineOption dest_option(
                QStringList() << "e" << "dest",
                QApplication::translate("main", "Specify the destination files prefix name"),
                QApplication::translate("main", "dest")
                );
    command_line_parser.addOption(dest_option);

    QCommandLineOption goil_option(
                QStringList() << "o" << "goil",
                QApplication::translate("main", "Specify the goil executable file path"),
                QApplication::translate("main", "goil")
                );
    command_line_parser.addOption(goil_option);

    QCommandLineOption out_dir_option(
                QStringList() << "r" << "outdir",
                QApplication::translate("main", "Specify the output directory path"),
                QApplication::translate("main", "outdir")
                );
    command_line_parser.addOption(out_dir_option);

    QCommandLineOption key_map_option(
                QStringList() << "k" << "keymap",
                QApplication::translate("main", "Specify the key mapping in the format {<key_handler>:<pin_number>,...}\n"
                                                "Example: {OnKey_i:4, OnKey_d:5}"),
                QApplication::translate("main", "keymapping")
                );
    command_line_parser.addOption(key_map_option);

    QCommandLineOption flash_option(
                QStringList() << "f" << "flash",
                QApplication::translate("main", "Flash binaries to arduino on COM port")
                );
    command_line_parser.addOption(flash_option);

    QCommandLineOption com_port_option(
                QStringList() << "p" << "port",
                QApplication::translate("main", "Must be set if it choosed to flash binaries (-f, --flash). Specify the COM port which the arduino is connected to"),
                QApplication::translate("main", "comport")
                );
    command_line_parser.addOption(com_port_option);

    QCommandLineOption arduino_nano_option(
                QStringList() << "a" << "arduinonano",
                QApplication::translate("main", "Must be set if it choosed to flash binaries (-f, --flash). Indicates if the arduino board is an arduino NANO")
                );
    command_line_parser.addOption(arduino_nano_option);

    command_line_parser.process(app);

    auto options = command_line_parser.optionNames();

    if(command_line_parser.isSet(gui_option) ||
       std::all_of(options.begin(),
                   options.end(),
                   [&command_line_parser](QString& opt)
                   {
                        return !command_line_parser.isSet(opt);
                   }))
    {
        CodeGeneratorWizard w;
        w.show();
        return app.exec();
    } else
    {
        if(!command_line_parser.isSet(cpp_src_option) ||
           !command_line_parser.isSet(def_option) ||
           !command_line_parser.isSet(dbf_option) ||
           !command_line_parser.isSet(trampoline_option) ||
           !command_line_parser.isSet(dest_option) ||
           !command_line_parser.isSet(goil_option) ||
           !command_line_parser.isSet(out_dir_option))
        {
            qDebug() << "Error! If -g or --gui option is not set, must specifiy at least all of these otptions:\n"
                     << "\t- cppsrc\n"
                     << "\t- def\n"
                     << "\t- DBF\n"
                     << "\t- trampolineroot\n"
                     << "\t- dest\n"
                     << "\t- goil\n"
                     << "\t- outdir\n";
            command_line_parser.showHelp();
        } else
        {
            auto cpp_src = command_line_parser.value(cpp_src_option);
            auto def_src = command_line_parser.value(def_option);
            auto dbf_src = command_line_parser.value(dbf_option);
            auto trampoline_root_path = command_line_parser.value(trampoline_option);
            auto dest_prefix_name = command_line_parser.value(dest_option);
            auto goil_path = command_line_parser.value(goil_option);
            auto out_dir_path = command_line_parser.value(out_dir_option);
            auto flash = command_line_parser.isSet(flash_option);
            auto key_mapping = command_line_parser.value(key_map_option);
            auto arduino_nano = command_line_parser.isSet(arduino_nano_option);
            auto com_port = command_line_parser.value(com_port_option);

            QFileInfo cpp_src_file_info(cpp_src);
            if(cpp_src_file_info.completeSuffix() != "cpp")
            {
                qDebug() << "Error: <cppsrc> file must have .cpp extension!\n";
                return 0;
            } else if(!cpp_src_file_info.exists())
            {
                qDebug() << "Error: <cppsrc> file does not exist!\n";
                return 0;
            }

            QFileInfo def_src_file_info(def_src);
            if(def_src_file_info.completeSuffix() != "def")
            {
                qDebug() << "Error: <def> file must have .def extension!\n";
                return 0;
            } else if(!def_src_file_info.exists())
            {
                qDebug() << "Error: <def> file does not exist!\n";
                return 0;
            }

            QFileInfo dbf_src_file_info(dbf_src);
            if(dbf_src_file_info.completeSuffix() != "DBF")
            {
                qDebug() << "Error: <DBF> file must have .DBF extension!\n";
                return 0;
            } else if(!dbf_src_file_info.exists())
            {
                qDebug() << "Error: <DBF> file does not exist!\n";
                return 0;
            }

            QFileInfo trampoline_root_dir_info(trampoline_root_path);
            if(!trampoline_root_dir_info.isDir())
            {
                qDebug() << "Error: <trampolineroot> is not a directory!\n";
                return 0;
            }

            QFileInfo out_dir_info(out_dir_path);
            if(!out_dir_info.isDir())
            {
                qDebug() << "Error: <outdir> is not a directory!\n";
                return 0;
            }

            QFileInfo goil_file_info(goil_path);
            if(goil_file_info.completeSuffix() != "exe")
            {
                qDebug() << "Error: <goil> file must have .exe extension!\n";
                return 0;
            } else if(!goil_file_info.exists())
            {
                qDebug() << "Error: <goil> file does not exist!\n";
                return 0;
            }

            auto available_com_ports = QSerialPortInfo::availablePorts();
            if(std::find_if(available_com_ports.begin(),
                         available_com_ports.end(),
                         [&com_port](QSerialPortInfo& com_port_info)
                         {
                            return com_port_info.portName() == com_port;
                         })
                    != available_com_ports.end())
            {
                qDebug() << "Error: <comport> ([" << com_port << "]) is not available!\n";
                return 0;
            }

            code_generator::CodeGenerator cd;
            code_generator::CodeGeneratorPropertiesManager(cd)
                    .set_def_file_path(def_src)
                    .set_dbf_file_path(dbf_src)
                    .set_output_oil_file_name(dest_prefix_name + ".oil")
                    .set_trampoline_root_path(trampoline_root_path)
                    .set_output_cpp_file_name(dest_prefix_name + ".cpp")
                    .set_output_exe_file_name(dest_prefix_name + "_bin")
                    .set_output_msg_types_header_file_name("msg_types.h")
                    .set_cpp_src_file_path(cpp_src)
                    .set_output_folder_path(out_dir_path)
                    .set_goil_exe_path(goil_path)
                    .set_key_mapping(key_mapping.toStdString())
                    .set_com_port(com_port)
                    .is_arduino_nano(arduino_nano)
                    .flash(flash)
                    .configure();
            cd.execute();
            return 0;
        }
    }
}
