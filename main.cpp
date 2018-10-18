#include "code_generator_wizard.h"
#include <QApplication>
#include <QDebug>
#include <QCommandLineParser>
#include <QFileInfo>
#include <code_generator.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("code_generator");
    QApplication::setApplicationName("Code Generator");
    QApplication::setApplicationVersion("0.1");

    QCommandLineParser command_line_parser;

    command_line_parser.setApplicationDescription("Este programa serve para gerar código fonte e executável para "
                                                  "plataforma Arduino com o sistema operacional de tempo real "
                                                  "TrampolineRTOS a partir de códigos fonte de simulação gerados pelo "
                                                  "Busmaster. Além disso, também é capaz de transferir o executável"
                                                  "para a plataforma através do programa avrdude. Faz uso das "
                                                  "ferramentas do TrampolineRTOS como goil e o build system com os"
                                                  "scripts em python, além de utilizar a toolchain do GNU para AVR."
                                                  "Alguns requisitos para o funcionamento dessa aplicação sao:\n"
                                                  "\t* A variável de ambiente PYTHONPATH que deve incluir a lista de "
                                                  "diretorios dentro do diretorio root da instalacao do python. "
                                                  R"(Exemplo: PYTHONPATH=C:\Python27\DLLs;C:\Python27\Doc;)"
                                                  R"(C:\Python27\Lib;C:\Python27\libs;C:\Python27\include;)"
                                                  R"(C:\Python27\tcl;C:\Python27\Tools)" "\n"
                                                  "\t* A varíavel de ambiente PATH deve conter os diretórios de binários"
                                                  " da toolchain e do python. " R"(Exemplo: Path=...;C:\WinAVR-20100110\bin;)"
                                                  R"(C:\WinAVR-20100110\utils\bin;)" "\n"
                                                  R"(C:\Python27;)" "\n");
    command_line_parser.addHelpOption();
    command_line_parser.addVersionOption();

    command_line_parser.addPositionalArgument("cpp_source_file", QApplication::translate("main", "Arquivo de código fonte .cpp gerado pelo Busmaster"));
    command_line_parser.addPositionalArgument("def_file", QApplication::translate("main", "Arquivo de definições .def gerado pelo Busmaster"));
    command_line_parser.addPositionalArgument("dbf_file", QApplication::translate("main", "Arquivo do banco de dados de mensagens .DBF gerado pelo Busmaster"));
    command_line_parser.addPositionalArgument("trampoline_root", QApplication::translate("main", "Caminho do diretório base do TrampolineRTOS"));
    command_line_parser.addPositionalArgument("destination_prefix", QApplication::translate("main", "Nome prefixo dos arquivos de saída"));
    command_line_parser.addPositionalArgument("goil_exe", QApplication::translate("main", "Caminho para arquivo executável do GOIL"));
    command_line_parser.addPositionalArgument("out_dir", QApplication::translate("main", "Caminho para o diretório de saída"));

    QCommandLineOption key_map_option(
                QStringList() << "k" << "keymap",
                QApplication::translate("main", "Mapeamento de teclas para pinos no formato: {<key_handler>:<pin_number>,...}\n"
                                                "Exemplo: {OnKey_i:4, OnKey_d:5}"),
                QApplication::translate("main", "keymapping")
                );
    command_line_parser.addOption(key_map_option);

    QCommandLineOption flash_option(
                QStringList() << "f" << "flash",
                QApplication::translate("main", "Importar binários para a plataforma arduino conectada a <comport>")
                );
    command_line_parser.addOption(flash_option);

    QCommandLineOption com_port_option(
                QStringList() << "p" << "port",
                QApplication::translate("main", "Deve ser fornecido caso deseje importar os binários para o arduiuno"
                                                " (-f, --flash). "
                                                "Indica a porta COM conectada ao arduino"),
                QApplication::translate("main", "comport")
                );
    command_line_parser.addOption(com_port_option);

    QCommandLineOption arduino_nano_option(
                QStringList() << "a" << "arduinonano",
                QApplication::translate("main", "Indica que a placa arduino utilizada é do tipo NANO, caso a opção não"
                                                " seja fornecida assume-se que a placa utiliazda é do tipo UNO")
                );
    command_line_parser.addOption(arduino_nano_option);

    command_line_parser.process(app);

    const QStringList positionalArguments = command_line_parser.positionalArguments();

    if(positionalArguments.isEmpty())
    {
        CodeGeneratorWizard w;
        w.show();
        return app.exec();
    } else
    {
        if(positionalArguments.size() < 7)
        {
            qDebug() << "Erro! Necessário especificar todos os argumentos. "
                        "Para utilizar a interface gráfica, basta executar o programa sem nenhum argumento";
            command_line_parser.showHelp();
        } else
        {
            auto cpp_src = positionalArguments.at(0);
            auto def_src = positionalArguments.at(1);
            auto dbf_src = positionalArguments.at(2);
            auto trampoline_root_path = positionalArguments.at(3);
            auto dest_prefix_name = positionalArguments.at(4);
            auto goil_path = positionalArguments.at(5);
            auto out_dir_path = positionalArguments.at(6);
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
            if(flash && std::find_if(available_com_ports.begin(),
                         available_com_ports.end(),
                         [&com_port](QSerialPortInfo& com_port_info)
                         {
                            return com_port_info.portName() == com_port;
                         })
                    == available_com_ports.end())
            {
                qDebug() << "Error: <comport> [" << com_port << "] is not available!\n";
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
                    .is_can_sender(true)
                    .use_serial_interface(true)
                    .flash(flash)
                    .configure();
            auto log = cd.execute();
            qDebug() << log;
            return 0;
        }
    }
}
