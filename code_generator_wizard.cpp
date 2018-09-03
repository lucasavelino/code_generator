#include <QtWidgets>
#include <QVector>
#include <QPair>
#include "code_generator_wizard.h"

CodeGeneratorWizard::CodeGeneratorWizard(QWidget *parent) :
    QWizard(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setButtonText(WizardButton::NextButton,tr("&Avançar >"));
    setButtonText(WizardButton::BackButton,tr("< &Voltar"));
    setButtonText(WizardButton::CancelButton,tr("&Cancelar"));
    setButtonText(WizardButton::FinishButton,tr("&Finalizar"));
    setWindowIcon(QIcon(":/images/cg_icon.png"));
    auto *intro_page = new IntroPage;
    auto *busmaster_generated_input_files_page = new BusmasterGeneratedInputFilesPage;
    auto *trampoline_rtos_configs_page = new TrampolineRTOSConfigsPage;
    auto *output_configs_page = new OutputConfigsPage;
    auto *pins_config_page = new PinsConfigPage;
    auto *build_page = new BuildPage;
    auto *com_port_page = new ComPortPage;
    auto *load_page = new LoadPage;
    auto *last_page = new LastPage;
    addPage(intro_page);
    addPage(busmaster_generated_input_files_page);
    addPage(trampoline_rtos_configs_page);
    addPage(output_configs_page);
    addPage(pins_config_page);
    addPage(build_page);
    addPage(com_port_page);
    addPage(load_page);
    addPage(last_page);

    setWindowTitle(tr("Code Generator"));
}

void CodeGeneratorWizard::accept()
{
    QWizard::accept();
}

IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Bem vindo"));
    label = new QLabel(tr("Esse assistente serve para realizar as configurações do Code Generator"));
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    setLayout(layout);
}

BusmasterGeneratedInputFilesPage::BusmasterGeneratedInputFilesPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Arquivos do busmaster"));
    setSubTitle(tr("Arquivos de codigo gerados pelo busmaster "
                   "a serem utilizados como entrada"));
    def_file_label = new QLabel(tr("Arquivo de definições (.def):"));
    def_file_label->setMinimumWidth(150);
    def_file_label->setWordWrap(true);
    def_file_line_edit = new QLineEdit;
    def_file_line_edit->setFixedWidth(300);
    def_file_open_button = new QPushButton(tr("Procurar..."));
    def_file_label->setBuddy(def_file_line_edit);
    connect(def_file_open_button,
            &QPushButton::clicked,
            this,
            [&]()
            {
                QString def_file_name = QFileDialog::getOpenFileName(
                                            this,
                                            tr("Procurar Arquivo"),
                                            QString(),
                                            "Arquivos DEF (*.def)"
                                        );
                def_file_line_edit->setText(def_file_name);
            });

    dbf_file_label = new QLabel(tr("Arquivo do banco de dados de mensagens (.DBF):"));
    dbf_file_label->setMinimumWidth(150);
    dbf_file_label->setWordWrap(true);
    dbf_file_line_edit = new QLineEdit;
    dbf_file_line_edit->setFixedWidth(300);
    dbf_file_open_button = new QPushButton(tr("Procurar..."));
    dbf_file_label->setBuddy(dbf_file_line_edit);
    connect(dbf_file_open_button,
            &QPushButton::clicked,
            this,
            [&]()
            {
                QString dbf_file_name = QFileDialog::getOpenFileName(
                                            this,
                                            tr("Procurar Arquivo"),
                                            QString(),
                                            "Arquivos DEF (*.DBF)"
                                        );
                dbf_file_line_edit->setText(dbf_file_name);
            });

    cpp_file_label = new QLabel(tr("Arquivo de código fonte (.cpp):"));
    cpp_file_label->setMinimumWidth(150);
    cpp_file_label->setWordWrap(true);
    cpp_file_line_edit = new QLineEdit;
    cpp_file_line_edit->setFixedWidth(300);
    cpp_file_open_button = new QPushButton(tr("Procurar..."));
    cpp_file_label->setBuddy(cpp_file_line_edit);
    connect(cpp_file_open_button,
            &QPushButton::clicked,
            this,
            [&]()
            {
                QString cpp_file_name = QFileDialog::getOpenFileName(
                                            this,
                                            tr("Procurar Arquivo"),
                                            QString(),
                                            "Arquivos DEF (*.cpp)"
                                        );
                cpp_file_line_edit->setText(cpp_file_name);
            });

    registerField("def_file*", def_file_line_edit);
    registerField("dbf_file*", dbf_file_line_edit);
    registerField("cpp_file*", cpp_file_line_edit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(def_file_label, 0, 0);
    layout->addWidget(def_file_line_edit, 0, 1);
    layout->addWidget(def_file_open_button, 0, 2);
    layout->addWidget(dbf_file_label, 1, 0);
    layout->addWidget(dbf_file_line_edit, 1, 1);
    layout->addWidget(dbf_file_open_button, 1, 2);
    layout->addWidget(cpp_file_label, 2, 0);
    layout->addWidget(cpp_file_line_edit, 2, 1);
    layout->addWidget(cpp_file_open_button, 2, 2);
    setLayout(layout);

}

bool BusmasterGeneratedInputFilesPage::validatePage()
{
    auto def_file = def_file_line_edit->text();
    if(!def_file.endsWith(".def"))
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo de definições deve possuir extensão .def"));
        msg.exec();
        return false;
    } else if(!QFileInfo(def_file).exists())
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo de definições: '%1' não foi encontrado").arg(def_file));
        msg.exec();
        return false;
    }

    auto dbf_file = dbf_file_line_edit->text();
    if(!dbf_file.endsWith(".DBF"))
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo do banco de dados de mensagens deve possuir extensão .DBF"));
        msg.exec();
        return false;
    } else if(!QFileInfo(dbf_file).exists())
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo do banco de dados de mensagens: '%1' não foi encontrado").arg(dbf_file));
        msg.exec();
        return false;
    }

    auto cpp_file = cpp_file_line_edit->text();
    if(!cpp_file.endsWith(".cpp"))
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo de código fonte deve possuir extensão .cpp"));
        msg.exec();
        return false;
    } else if(!QFileInfo(cpp_file).exists())
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo de código fonte: '%1' não foi encontrado").arg(cpp_file));
        msg.exec();
        return false;
    }

    return true;
}

TrampolineRTOSConfigsPage::TrampolineRTOSConfigsPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Configurações do TrampolineRTOS"));
    setSubTitle(tr("Indicar configurações do trampolineRTOS"));

    trampoline_root_label = new QLabel(tr("Diretório base do trampolineRTOS:"));
    trampoline_root_label->setMinimumWidth(150);
    trampoline_root_label->setWordWrap(true);
    trampoline_root_line_edit = new QLineEdit;
    trampoline_root_line_edit->setFixedWidth(300);
    trampoline_root_open_button = new QPushButton(tr("Procurar..."));
    trampoline_root_label->setBuddy(trampoline_root_line_edit);
    connect(trampoline_root_open_button,
            &QPushButton::clicked,
            this,
            [&]()
            {
                QString trampoline_root_dir_name = QFileDialog::getExistingDirectory(
                                            this,
                                            tr("Procurar Diretório"),
                                            QString(),
                                            QFileDialog::ShowDirsOnly
                                            | QFileDialog::DontResolveSymlinks
                                        );
                trampoline_root_line_edit->setText(trampoline_root_dir_name);
            });

    goil_exe_label = new QLabel(tr("Executável do GOIL:"));
    goil_exe_label->setMinimumWidth(150);
    goil_exe_label->setWordWrap(true);
    goil_exe_line_edit = new QLineEdit;
    goil_exe_line_edit->setFixedWidth(300);
    goil_exe_open_button = new QPushButton(tr("Procurar..."));
    goil_exe_label->setBuddy(goil_exe_line_edit);
    connect(goil_exe_open_button,
            &QPushButton::clicked,
            this,
            [&]()
            {
                QString goil_exe_dir_name = QFileDialog::getOpenFileName(
                                            this,
                                            tr("Procurar Arquivo"),
                                            QString(),
                                            tr("Arquivos executáveis (*.exe)")
                                        );
                goil_exe_line_edit->setText(goil_exe_dir_name);
            });

    registerField("trampoline_root_dir*", trampoline_root_line_edit);
    registerField("goil_exe_file*", goil_exe_line_edit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(trampoline_root_label, 0, 0);
    layout->addWidget(trampoline_root_line_edit, 0, 1);
    layout->addWidget(trampoline_root_open_button, 0, 2);
    layout->addWidget(goil_exe_label, 1, 0);
    layout->addWidget(goil_exe_line_edit, 1, 1);
    layout->addWidget(goil_exe_open_button, 1, 2);
    setLayout(layout);
}

bool TrampolineRTOSConfigsPage::validatePage()
{
    auto trampoline_root_dir = trampoline_root_line_edit->text();
    auto trampoline_root_dir_file_info = QFileInfo(trampoline_root_dir);
    if(!trampoline_root_dir_file_info.exists())
    {
        QMessageBox msg;
        msg.setText(tr("O diretório base do trampolineRTOS: '%1' não foi encontrado").arg(trampoline_root_dir));
        msg.exec();
        return false;
    } else if(!trampoline_root_dir_file_info.isDir())
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo: '%1' não é um diretório").arg(trampoline_root_dir));
        msg.exec();
        return false;
    }

    auto goil_exe = goil_exe_line_edit->text();
    if(!goil_exe.endsWith(".exe"))
    {
        QMessageBox msg;
        msg.setText(tr("O executável do GOIL deve possuir extensão .exe"));
        msg.exec();
        return false;
    } else if(!QFileInfo(goil_exe).exists())
    {
        QMessageBox msg;
        msg.setText(tr("O executável do GOIL: '%1' não foi encontrado").arg(goil_exe));
        msg.exec();
        return false;
    }
    return true;
}

OutputConfigsPage::OutputConfigsPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Configurações de saída"));
    setSubTitle(tr("Indicar configurações dos resultados produzidos pelo Code Generator"));

    output_folder_label = new QLabel(tr("Diretório de saída:"));
    output_folder_label->setMinimumWidth(150);
    output_folder_label->setWordWrap(true);
    output_folder_line_edit = new QLineEdit;
    output_folder_line_edit->setFixedWidth(300);
    output_folder_open_button = new QPushButton(tr("Procurar..."));
    output_folder_label->setBuddy(output_folder_line_edit);
    connect(output_folder_open_button,
            &QPushButton::clicked,
            this,
            [&]()
            {
                QString output_folder_dir_name = QFileDialog::getExistingDirectory(
                                            this,
                                            tr("Procurar Diretório"),
                                            QString(),
                                            QFileDialog::ShowDirsOnly
                                            | QFileDialog::DontResolveSymlinks
                                        );
                output_folder_line_edit->setText(output_folder_dir_name);
            });

    output_prefix_label = new QLabel(tr("Nome prefixo do arquivo de saida (sem extensão):"));
    output_prefix_label->setMinimumWidth(150);
    output_prefix_label->setWordWrap(true);
    output_prefix_line_edit = new QLineEdit;
    output_prefix_line_edit->setFixedWidth(300);
    output_prefix_label->setBuddy(output_prefix_line_edit);

    registerField("output_dir*", output_folder_line_edit);
    registerField("output_prefix_name*", output_prefix_line_edit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(output_folder_label, 0, 0);
    layout->addWidget(output_folder_line_edit, 0, 1);
    layout->addWidget(output_folder_open_button, 0, 2);
    layout->addWidget(output_prefix_label, 1, 0);
    layout->addWidget(output_prefix_line_edit, 1, 1);
    setLayout(layout);
}

bool OutputConfigsPage::validatePage()
{
    auto output_folder = output_folder_line_edit->text();
    auto output_folder_file_info = QFileInfo(output_folder);
    if(!output_folder_file_info.exists())
    {
        QMessageBox msg;
        msg.setText(tr("O diretório de saída: '%1' não foi encontrado").arg(output_folder));
        msg.exec();
        return false;
    } else if(!output_folder_file_info.isDir())
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo: '%1' não é um diretório").arg(output_folder));
        msg.exec();
        return false;
    }

    auto output_prefix = output_prefix_line_edit->text();
    if(output_prefix.contains("."))
    {
        QMessageBox msg;
        msg.setText(tr("O nome prefixo do arquivo de saida não deve conter extensão"));
        msg.exec();
        return false;
    }

    return true;
}

PinsConfigPage::PinsConfigPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Configuração dos pinos"));
    setSubTitle(tr("Associar os pinos do arduino com os métodos OnKey gerados pelo busmaster"));

    arduino_select = new QComboBox;
    arduino_select->addItem(tr("Arduino UNO"));
    arduino_select->addItem(tr("Arduino NANO"));
    arduino_img_label = new QLabel;
    arduino_img_label->setPixmap(QPixmap(":/images/arduino_uno.jpg")
                                 .scaled(345,
                                        448,
                                        Qt::KeepAspectRatio
                                        )
                                 );

    connect(arduino_select,
            QOverload<int>::of(&QComboBox::activated),
            [&](int index)
            {
                if(index == 0)
                {
                    arduino_img_label->setPixmap(QPixmap(":/images/arduino_uno.jpg")
                                                 .scaled(345,
                                                        448,
                                                        Qt::KeepAspectRatio
                                                        )
                                                 );
                } else
                {
                    arduino_img_label->setPixmap(QPixmap(":/images/arduino_nano.jpg")
                                                 .scaled(185,
                                                        358,
                                                        Qt::KeepAspectRatio
                                                        )
                                                 );
                }

            });

    for (int i = 0; i < 20; ++i)
    {
        arduino_pins_label[i] = new QLabel(QString("Associar pino %1:").arg(QString::number(i)));
        arduino_pins_checkbox[i] = new QCheckBox;
        arduino_pins_line_edit[i] = new QLineEdit;
        arduino_pins_label[i]->setBuddy(arduino_pins_checkbox[i]);
        keys_label[i] = new QLabel(tr("à tecla:"));
        keys_label[i]->setBuddy(arduino_pins_line_edit[i]);
        arduino_pins_line_edit[i]->setEnabled(false);
        connect(arduino_pins_checkbox[i],
                &QCheckBox::toggled,
                this,
                [this,i](bool state)
                {
                    arduino_pins_line_edit[i]->setEnabled(state);
                    if(!state)
                    {
                        arduino_pins_line_edit[i]->clear();
                    }
                });
        registerField(QString("is_pin_%1_associated_to_a_key").arg(QString::number(i)), arduino_pins_checkbox[i]);
        registerField(QString("pin_%1_key").arg(QString::number(i)), arduino_pins_line_edit[i]);
    }

    setButtonText(QWizard::NextButton, tr("C&ompilar"));

    auto *layout = new QGridLayout;
    layout->addWidget(arduino_select, 0, 0);
    layout->addWidget(arduino_img_label, 0, 8, 20, 1);
    for (int i = 0; i < 20; ++i)
    {
        layout->addWidget(arduino_pins_label[i], (i % 10) + 1, ((i / 10) * 4) + 0);
        layout->addWidget(arduino_pins_checkbox[i], (i % 10) + 1, ((i / 10) * 4) + 1);
        layout->addWidget(keys_label[i], (i % 10) + 1, ((i / 10) * 4) + 2);
        layout->addWidget(arduino_pins_line_edit[i], (i % 10) + 1, ((i / 10) * 4) + 3);
    }
    setLayout(layout);
}

BuildPage::BuildPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Geração e compilação do código fonte"));
    setSubTitle(tr("Veja o resultado do processo de geração do código e compilação"));

    build_text_edit = new QTextEdit;
    build_text_edit->setReadOnly(true);
    registerField("build_text_edit",build_text_edit,"plainText","textChanged");
    auto *layout = new QVBoxLayout;
    layout->addWidget(build_text_edit);
    setLayout(layout);
}

void BuildPage::initializePage()
{
    auto def_file = field("def_file*").toString();
    auto dbf_file = field("dbf_file*").toString();
    auto cpp_file = field("cpp_file*").toString();

    auto trampoline_root_dir = field("trampoline_root_dir*").toString();
    auto goil_exe_file = field("goil_exe_file*").toString();

    auto output_dir = field("output_dir*").toString();
    auto output_prefix_name = field("output_prefix_name*").toString();

    QString pins_associated_to_keys = "{";
    bool first_pin = true;
    for(int i = 0; i < 20; ++i)
    {
        if(field(QString("is_pin_%1_associated_to_a_key").arg(QString::number(i)))
                .toBool())
        {
            if(first_pin)
            {
                first_pin = false;
            } else
            {
                pins_associated_to_keys += ", ";
            }
            pins_associated_to_keys += "OnKey_" + field(QString("pin_%1_key").arg(QString::number(i))).toString()
                    + ":" + QString::number(i);
        }
    }
    pins_associated_to_keys += "}";

    code_generator::CodeGenerator cd;
    code_generator::CodeGeneratorPropertiesManager(cd)
            .set_def_file_path(def_file)
            .set_dbf_file_path(dbf_file)
            .set_cpp_src_file_path(cpp_file)
            .set_trampoline_root_path(trampoline_root_dir)
            .set_goil_exe_path(goil_exe_file)
            .set_output_folder_path(output_dir)
            .set_output_cpp_file_name(output_prefix_name + ".cpp")
            .set_output_oil_file_name(output_prefix_name + ".oil")
            .set_output_msg_types_header_file_name("msg_types.h")
            .set_output_exe_file_name(output_prefix_name + "_bin")
            .set_key_mapping(pins_associated_to_keys.toStdString())
            .configure();
    QString build_output_str = cd.execute_build();
    setField("build_text_edit",build_output_str);
}


ComPortPage::ComPortPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Carregar executável"));
    setSubTitle(tr("Configuração para carregar executável na placa arduino"));

    com_port_label = new QLabel(tr("Seleciona a porta serial conectada a placa arduino"));
    com_port_label->setMinimumWidth(150);
    com_port_label->setWordWrap(true);
    com_port_select = new QComboBox;
    com_port_label->setBuddy(com_port_select);

    auto available_ports = QSerialPortInfo::availablePorts();
    for(const auto& port : available_ports)
    {
        com_port_select->addItem(port.portName());
    }

    com_ports_check_timer = new QTimer(this);
    connect(com_ports_check_timer,
            &QTimer::timeout,
            this,
            [this]()
            {
                auto current_text = com_port_select->currentText();
                com_port_select->clear();
                auto available_ports = QSerialPortInfo::availablePorts();
                for(const auto& port : available_ports)
                {
                    com_port_select->addItem(port.portName());
                }
                auto new_com_port_selected_index = com_port_select->findText(current_text);
                if(new_com_port_selected_index != -1)
                {
                    com_port_select->setCurrentIndex(new_com_port_selected_index);
                }
            });
    com_ports_check_timer->start(3000);

    registerField("com_port", com_port_select);

    setButtonText(QWizard::NextButton, tr("Carregar &executável"));

    auto *layout = new QGridLayout;
    layout->addWidget(com_port_label, 0, 0);
    layout->addWidget(com_port_select, 0, 1);
    setLayout(layout);
}

LoadPage::LoadPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Geração e compilação do código fonte"));
    setSubTitle(tr("Veja o resultado do processo de geração do código e compilação"));

    load_text_edit = new QTextEdit;
    load_text_edit->setReadOnly(true);
    registerField("load_text_edit",load_text_edit,"plainText","textChanged");
    auto *layout = new QVBoxLayout;
    layout->addWidget(load_text_edit);
    setLayout(layout);
}

void LoadPage::initializePage()
{
    auto output_dir = field("output_dir*").toString();
    auto output_prefix_name = field("output_prefix_name*").toString();
    auto com_port = field("com_port").toString();

    code_generator::CodeGenerator cd;
    code_generator::CodeGeneratorPropertiesManager(cd)
            .set_output_folder_path(output_dir)
            .set_output_exe_file_name(output_prefix_name + "_bin")
            .set_com_port(com_port);
    QString load_output_str = cd.execute_flash();
    setField("load_text_edit",load_output_str);
}

LastPage::LastPage(QWidget *parent)
    : QWizardPage(parent)
{

    setTitle(tr("Fim"));
    label = new QLabel(tr("O assistente terminou o serviço!"));
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    setLayout(layout);
}
