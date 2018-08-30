#include <QtWidgets>

#include "code_generator_wizard.h"

CodeGeneratorWizard::CodeGeneratorWizard(QWidget *parent) :
    QWizard(parent)
{
    setButtonText(WizardButton::NextButton,tr("&Avançar >"));
    setButtonText(WizardButton::BackButton,tr("< &Voltar"));
    setButtonText(WizardButton::CancelButton,tr("&Cancelar"));
    setButtonText(WizardButton::FinishButton,tr("&Finalizar"));
    setWizardStyle(WizardStyle::AeroStyle);
    addPage(new IntroPage);
    addPage(new BusmasterGeneratedInputFilesPage);
    addPage(new TrampolineRTOSConfigsPage);
    addPage(new OutputConfigsPage);
    addPage(new PinsConfigPage);
    addPage(new BuildPage);
    addPage(new LoadPage);
    addPage(new LastPage);

    setWindowTitle(tr("Code Generator"));
}

void CodeGeneratorWizard::accept()
{

}

IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Bem vindo"));
    label = new QLabel(tr("Esse wizard serve para realizar as configurações do Code Generator"));
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

    cpp_file_label = new QLabel(tr("Arquivo de codigo fonte (.cpp):"));
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

    registerField("def_file", def_file_line_edit);
    registerField("dbf_file", dbf_file_line_edit);
    registerField("cpp_file", cpp_file_line_edit);

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

    registerField("trampoline_root_dir", trampoline_root_line_edit);
    registerField("goil_exe_file", goil_exe_line_edit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(trampoline_root_label, 0, 0);
    layout->addWidget(trampoline_root_line_edit, 0, 1);
    layout->addWidget(trampoline_root_open_button, 0, 2);
    layout->addWidget(goil_exe_label, 1, 0);
    layout->addWidget(goil_exe_line_edit, 1, 1);
    layout->addWidget(goil_exe_open_button, 1, 2);
    setLayout(layout);
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

    oil_file_label = new QLabel(tr("Nome do arquivo OIL (.oil):"));
    oil_file_label->setMinimumWidth(150);
    oil_file_label->setWordWrap(true);
    oil_file_line_edit = new QLineEdit;
    oil_file_line_edit->setFixedWidth(300);
    oil_file_label->setBuddy(oil_file_line_edit);

    cpp_file_label = new QLabel(tr("Nome do arquivo CPP (.cpp):"));
    cpp_file_label->setMinimumWidth(150);
    cpp_file_label->setWordWrap(true);
    cpp_file_line_edit = new QLineEdit;
    cpp_file_line_edit->setFixedWidth(300);
    cpp_file_label->setBuddy(cpp_file_line_edit);

    msg_types_header_file_label = new QLabel(tr("Nome do arquivo de cabeçalho das mensagens do banco de dados (.h):"));
    msg_types_header_file_label->setMinimumWidth(150);
    msg_types_header_file_label->setWordWrap(true);
    msg_types_header_file_line_edit = new QLineEdit;
    msg_types_header_file_line_edit->setFixedWidth(300);
    msg_types_header_file_label->setBuddy(msg_types_header_file_line_edit);

    registerField("output_dir", output_folder_line_edit);
    registerField("oil_file_name", oil_file_line_edit);
    registerField("cpp_file_name", cpp_file_line_edit);
    registerField("msg_types_header_file_name", msg_types_header_file_line_edit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(output_folder_label, 0, 0);
    layout->addWidget(output_folder_line_edit, 0, 1);
    layout->addWidget(output_folder_open_button, 0, 2);
    layout->addWidget(oil_file_label, 1, 0);
    layout->addWidget(oil_file_line_edit, 1, 1);
    layout->addWidget(cpp_file_label, 2, 0);
    layout->addWidget(cpp_file_line_edit, 2, 1);
    layout->addWidget(msg_types_header_file_label, 3, 0);
    layout->addWidget(msg_types_header_file_line_edit, 3, 1);
    setLayout(layout);
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
    arduino_img_label->setPixmap(QPixmap(":/images/arduino_uno.jpg").
                                 scaled(345,
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
                    arduino_img_label->setPixmap(QPixmap(":/images/arduino_uno.jpg").
                                                 scaled(345,
                                                        448,
                                                        Qt::KeepAspectRatio
                                                        )
                                                 );
                } else
                {
                    arduino_img_label->setPixmap(QPixmap(":/images/arduino_nano.jpg").
                                                 scaled(185,
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
    layout->addWidget(arduino_img_label, 0, 4, 20, 1);
    for (int i = 0; i < 20; ++i)
    {
        layout->addWidget(arduino_pins_label[i], i+1, 0);
        layout->addWidget(arduino_pins_checkbox[i], i+1, 1);
        layout->addWidget(keys_label[i], i+1, 2);
        layout->addWidget(arduino_pins_line_edit[i], i+1, 3);
    }
    setLayout(layout);
}

BuildPage::BuildPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Geração e compilação do código fonte"));
    setSubTitle(tr("Veja o resultado do processo de geração do código e compilação"));

    build_text_browser = new QTextBrowser;
    registerField("build_text_browser",build_text_browser);
    auto *layout = new QVBoxLayout;
    layout->addWidget(build_text_browser);
    setLayout(layout);
}


LoadPage::LoadPage(QWidget *parent)
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

    load_text_browser = new QTextBrowser;

    registerField("com_port", com_port_select);
    registerField("load_text_browser", load_text_browser);

    setButtonText(QWizard::NextButton, tr("Carregar &executável"));

    auto *layout = new QGridLayout;
    layout->addWidget(com_port_label, 0, 0);
    layout->addWidget(com_port_select, 0, 1);
    layout->addWidget(load_text_browser, 1, 0, 1, 2);
    setLayout(layout);
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
